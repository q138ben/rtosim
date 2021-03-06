/* -------------------------------------------------------------------------- *
 * Copyright (c) 2010-2016 C. Pizzolato, M. Reggiani                          *
 *                                                                            *
 * Licensed under the Apache License, Version 2.0 (the "License");            *
 * you may not use this file except in compliance with the License.           *
 * You may obtain a copy of the License at:                                   *
 * http://www.apache.org/licenses/LICENSE-2.0                                 *
 *                                                                            *
 * Unless required by applicable law or agreed to in writing, software        *
 * distributed under the License is distributed on an "AS IS" BASIS,          *
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.   *
 * See the License for the specific language governing permissions and        *
 * limitations under the License.                                             *
 * -------------------------------------------------------------------------- */

#ifndef rtosim_ExternalForceData_h
#define rtosim_ExternalForceData_h

#include <SimTKcommon.h>
#include <ostream>
#include <utility>

namespace rtosim {
    class ExternalForceData {
    public:
        ExternalForceData();
        ExternalForceData(unsigned targetBodyIndex);
        void setTargetBody(unsigned targetBodyIndex);
        void setForceVector(const SimTK::Vec3& force, unsigned expressedInBodyIndex = 0);
        void setApplicationPoint(const SimTK::Vec3& point, unsigned expressedInBodyIndex = 0);
        void setTorque(const SimTK::Vec3& torque, unsigned expressedInBodyIndex = 0);
        void setMoments(const SimTK::Vec3& moments, unsigned expressedInBodyIndex = 0);
        void setSourceName(const std::string& name);
        std::string getSourceName() const;

        unsigned getTargetBodyIndex() const;

        const SimTK::Vec3& getForce() const;
        void getForce(SimTK::Vec3& force) const;
        unsigned getForceBodyIndex() const;

        void setUseApplicationPoint(bool useApplicationPoint); //if false applies grf to centre of mass
        bool getUseApplicationPoint() const;
        const SimTK::Vec3& getApplicationPoint() const;
        void getApplicationPoint(SimTK::Vec3& applicationPoint) const;
        unsigned getApplicationPointBodyIndex() const;

        const SimTK::Vec3& getMoments() const;
        void getMoments(SimTK::Vec3& moments) const;
        unsigned getMomentsBodyIndex() const;

        //torques should be updated based on moments, grf, and cop
        const SimTK::Vec3& getTorque() const;
        void getTorque(SimTK::Vec3& torque) const;
        unsigned getTorqueBodyIndex() const;

        void print(std::ostream& os) const;
    private:
        unsigned targetBody_;                     //target reference system indicesed to body
        std::pair<unsigned, SimTK::Vec3> point_;  //reference system index and point vector
        std::pair<unsigned, SimTK::Vec3> force_;  //reference system index and force vector
        std::pair<unsigned, SimTK::Vec3> torque_;  //reference system index and force vector
        std::pair<unsigned, SimTK::Vec3> moments_;  //reference system index and moments
        bool useApplicationPoint_;
        std::string sourceName_; //e.g. name of the force plates
    };
}

#endif