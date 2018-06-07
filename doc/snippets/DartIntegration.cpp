/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2018 Konstantinos Chatzilygeroudis <costashatz@gmail.com>

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

#include <dart/simulation/World.hpp>

#include "Magnum/SceneGraph/Scene.h"
#include "Magnum/SceneGraph/Object.h"
#include "Magnum/SceneGraph/MatrixTransformation3D.h"
#include "Magnum/DartIntegration/World.h"

using namespace Magnum;

dart::simulation::WorldPtr createWorldInDart();
void addDartSkeletonsToDartWorld();
template<class T> void deleteObjectsFromScene(T&);
template<class T> void updateMeshesAndMaterials(T&);

int main() {

{
/* [world-init] */
dart::simulation::WorldPtr dartWorld = createWorldInDart();
addDartSkeletonsToDartWorld();
SceneGraph::Scene<SceneGraph::MatrixTransformation3D> scene;
SceneGraph::Object<SceneGraph::MatrixTransformation3D> object{&scene};

/* object should never be a SceneGraph::Scene */
DartIntegration::World world{object, *dartWorld};
/* [world-init] */

UnsignedInt simulationSteps{};
/* [world-loop] */
/* Simulate with time step of 0.001 seconds */
world.world().setTimeStep(0.001);

for(UnsignedInt i = 0; i < simulationSteps; ++i) {
    world.step();

    /* Update graphics at ~60Hz (15*0.001 ~= 60Hz) */
    if(i % 15 == 0) {
        world.refresh();

        /* Get unused/deleted shapes */
        std::vector<std::unique_ptr<DartIntegration::Object>>& unusedObjects = world.unusedObjects();

        /* The user is expected to handle unused objects. One possible handling
           would be to remove them from the parent scene. */
        deleteObjectsFromScene(unusedObjects);

        /* Get updated shapes -- ones that either the materials or the meshes
          have changed */
        std::vector<std::reference_wrapper<DartIntegration::Object>> updatedObjects = world.updatedShapeObjects();

        updateMeshesAndMaterials(updatedObjects);

        /* Clear list of updated objects */
        world.clearUpdatedShapeObjects();
    }
}
/* [world-loop] */
}

}