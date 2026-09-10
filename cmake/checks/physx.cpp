#include <NxPhysics.h>
#include <NxCooking.h>
#include <PhysXLoader.h>
#include <cstdio>

int main()
{
	static_assert(sizeof(void *) == 4, "PhysX requires Win32");
	NxPhysicsSDKDesc sdk_desc;
	sdk_desc.flags = NX_SDKF_NO_HARDWARE;
	NxSDKCreateError error = NXCE_NO_ERROR;
	NxPhysicsSDK *sdk = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION,
		nullptr, nullptr, sdk_desc, &error);
	if (!sdk) {
		std::fprintf(stderr, "PhysX SDK initialization failed: %d\n",
			static_cast<int>(error));
		return 1;
	}
	if (!NxInitCooking()) {
		std::fputs("PhysX cooking initialization failed\n", stderr);
		NxReleasePhysicsSDK(sdk);
		return 1;
	}
	NxCloseCooking();
	NxSceneDesc scene_desc;
	scene_desc.gravity = NxVec3(0, -9.81f, 0);
	scene_desc.simType = NX_SIMULATION_SW;
	NxScene *scene = sdk->createScene(scene_desc);
	if (!scene) {
		NxReleasePhysicsSDK(sdk);
		return 1;
	}
	NxBoxShapeDesc shape;
	shape.dimensions = NxVec3(1, 1, 1);
	NxBodyDesc body;
	NxActorDesc actor_desc;
	actor_desc.shapes.pushBack(&shape);
	actor_desc.body = &body;
	actor_desc.density = 1;
	actor_desc.globalPose.t = NxVec3(0, 5, 0);
	NxActor *actor = scene->createActor(actor_desc);
	int failed = !actor;
	if (actor) {
		for (int step = 0; step < 30; ++step) {
			scene->simulate(1.0f / 60.0f);
			scene->flushStream();
			if (!scene->fetchResults(NX_RIGID_BODY_FINISHED, true))
				failed = 1;
		}
		float height = actor->getGlobalPosition().y;
		if (!(height > 3 && height < 4.5f))
			failed = 1;
		scene->releaseActor(*actor);
	}
	sdk->releaseScene(*scene);
	NxReleasePhysicsSDK(sdk);
	return failed;
}
