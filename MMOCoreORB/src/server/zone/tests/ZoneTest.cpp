/*
 * ZoneTest.cpp
 *
 *  Created on: 09/09/2013
 *      Author: victor
 */

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "server/zone/Zone.h"
#include "server/zone/ZoneProcessServer.h"
#include "server/zone/objects/scene/SceneObject.h"
#include "server/conf/ConfigManager.h"

class ZoneTest : public ::testing::Test {
protected:
	Reference<ZoneServer*> zoneServer;
	Reference<Zone*> zone;
	Reference<ZoneProcessServer*> processServer;

public:
	ZoneTest() {
		// Perform creation setup here.
	}

	~ZoneTest() {
		// Clean up.
	}

	void SetUp() {
		// Perform setup of common constructs here.
		ConfigManager::instance()->loadConfigData();
		ConfigManager::instance()->setProgressMonitors(false);
		zoneServer = new ZoneServer(ConfigManager::instance());
		processServer = new ZoneProcessServer(zoneServer);
		zone = new Zone(processServer, "test_zone");
		zone->createContainerComponent();
		zone->_setObjectID(1);
	}

	void TearDown() {
		// Perform clean up of common constructs here.
	}
};

TEST_F(ZoneTest, TreLoad) {
	TemplateManager::instance();

	ASSERT_EQ(TemplateManager::ERROR_CODE, 0);

	TemplateManager::instance()->loadLuaTemplates();

	ASSERT_EQ(TemplateManager::ERROR_CODE, 0);
}

TEST_F(ZoneTest, InRangeTest) {
	Reference<SceneObject*> scene = new SceneObject();
	scene->setContainerComponent("ContainerComponent");
	scene->setZoneComponent("ZoneComponent");
	scene->_setObjectID(2);

	zone->transferObject(scene, -1);

	ASSERT_TRUE(scene->getZone() != NULL);

	SortedVector<ManagedReference<QuadTreeEntry*> > objects;

	zone->getInRangeObjects(0, 0, 128, &objects, true);

	ASSERT_EQ(objects.size(), 1);

	scene->teleport(1000, 1000, 1000);

	objects.removeAll();

	zone->getInRangeObjects(0, 0, 128, &objects, true);

	ASSERT_EQ(objects.size(), 0);

	objects.removeAll();

	zone->getInRangeObjects(1000, 1000, 128, &objects, true);

	ASSERT_EQ(objects.size(), 1);

	Reference<SceneObject*> scene2 = new SceneObject();
	scene2->setContainerComponent("ContainerComponent");
	scene2->setZoneComponent("ZoneComponent");
	scene2->_setObjectID(3);
	scene2->setPosition(1000, 1000, 1000);

	zone->transferObject(scene2, -1);

	objects.removeAll();

	zone->getInRangeObjects(1000, 1000, 128, &objects, true);

	ASSERT_EQ(objects.size(), 2);

	scene2->destroyObjectFromWorld(false);
	scene->destroyObjectFromWorld(false);

	objects.removeAll();

	zone->getInRangeObjects(1000, 1000, 128, &objects, true);

	ASSERT_EQ(objects.size(), 0);
}


