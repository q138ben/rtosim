#include "rtosim/rtosim.h"
using namespace rtosim;

#include <iostream>
using std::cout;
using std::endl;
#include <string>
using std::string;
#include <Simbody.h>

void printAuthors() {

    cout << "Real-time OpenSim inverse kinematics" << endl;
    cout << "Authors: Claudio Pizzolato <claudio.pizzolato@griffithuni.edu.au>" << endl;
    cout << "         Monica Reggiani <monica.reggiani@unipd.it>" << endl << endl;
}

void printHelp() {

    printAuthors();

    auto progName(SimTK::Pathname::getThisExecutablePath());
    bool isAbsolute;
    string dir, filename, ext;
    SimTK::Pathname::deconstructPathname(progName, isAbsolute, dir, filename, ext);

    cout << "Option              Argument         Action / Notes\n";
    cout << "------              --------         --------------\n";
    cout << "-h                                   Print the command-line options for " << filename << ".\n";
    cout << "--model              ModelFilename    Specify the name of the osim model file for the investigation.\n";
    cout << "--mot                MotFilename      Specify the name of the mot file containing the results of IK.\n";
    cout << "--ext-loads          LoadsFilename    Specify the name of the XML ExternalLoads file.\n";
    cout << "--fc                 CutoffFrequency  Specify the name of lowpass cutoff frequency to filter IK data.\n";
    cout << "--output             OutputDir        Specify the output directory\n";
}

int main(int argc, char* argv[]) {

    ProgramOptionsParser po(argc, argv);
    if (po.exists("-h") || po.empty()) {
        printHelp();
        exit(EXIT_SUCCESS);
    }

    string osimModelFilename;
    if (po.exists("--model"))
        osimModelFilename = po.getParameter("--model");
    else {
        printHelp();
        exit(EXIT_SUCCESS);
    }

    string motTrialFilename;
    if (po.exists("--mot"))
        motTrialFilename = po.getParameter("--mot");
    else {
        printHelp();
        exit(EXIT_SUCCESS);
    }

    string externalLoadsXml;
    if (po.exists("--ext-loads"))
        externalLoadsXml = po.getParameter("--ext-loads");
    else {
        printHelp();
        exit(EXIT_SUCCESS);
    }

    double fc(8);
    if (po.exists("--fc"))
        fc = po.getParameter<double>("--fc");

    string resultDir("Output");
    if (po.exists("--output"))
        resultDir = po.getParameter("--output");
    FileSystem::createDirectory(resultDir);

    string stopWatchResultDir(resultDir);
    std::vector<string> dofLabels(getCoordinateNamesFromModel(osimModelFilename));

    MultipleExternalForcesQueue grfQueue;
    GeneralisedCoordinatesQueue coordinatesQueue;
    ExternalTorquesQueue jointMomentsQueue;
    rtosim::Concurrency::Latch doneWithSubscription, doneWithExecution;

    //prefilter the data
    ExternalForcesFromStorageFile grfProducer(
        grfQueue,
        doneWithSubscription,
        doneWithExecution,
        externalLoadsXml);

    //prefilter the data
    GeneralisedCoordinatesFromStorageFile ikProducer(
        coordinatesQueue,
        doneWithSubscription,
        doneWithExecution,
        osimModelFilename,
        motTrialFilename,
        fc);

    QueuesToInverseDynamics inverseDynamicsFromQueue(
        coordinatesQueue,
        grfQueue,
        jointMomentsQueue,
        doneWithSubscription,
        doneWithExecution,
        osimModelFilename,
        externalLoadsXml);

    QueueToFileLogger<ExternalTorquesData> inverseDynamicsFileLogger(
        jointMomentsQueue,
        doneWithSubscription,
        doneWithExecution,
        dofLabels,
        resultDir,
        "id",
        "sto");

    FrameCounter<ExternalTorquesQueue> idFrameCounter(jointMomentsQueue, "joint_moments_frame_counter");

    doneWithSubscription.setCount(4);
    doneWithExecution.setCount(4);

    QueuesSync::launchThreads(
        grfProducer,
        ikProducer,
        inverseDynamicsFromQueue,
        inverseDynamicsFileLogger,
        idFrameCounter
        );

    idFrameCounter.getProcessingTimes().print(stopWatchResultDir);

    return 0;
}