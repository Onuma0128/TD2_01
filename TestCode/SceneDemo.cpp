#include "SceneDemo.h"

#include "Engine/Game/Camera/Camera3D.h"
#include "Engine/Game/GameObject/GameObject.h"
#include "Engine/Game/Hierarchy/Hierarchy.h"
#include "Engine/Game/Managers/PolygonMeshManager/PolygonMeshManager.h"
#include "Engine/Game/Managers/SceneManager/SceneManager.h"
#include "Engine/Render/RenderPathManager/RenderPathManager.h"
#include "Engine/Utility/Utility.h"
#include "Engine/Game/Collision/Collider/SphereCollider.h"
#include "Engine/Game/Collision/CollisionManager/CollisionManager.h"

#include "Engine/Game/Color/Color.h"

SceneDemo::SceneDemo() = default;

SceneDemo::~SceneDemo() = default;

void SceneDemo::load() {
	PolygonMeshManager::RegisterLoadQue("./Engine/Resources/", "Sphere.obj");
}

void SceneDemo::initialize() {
	camera3D = CreateUnique<Camera3D>();
	camera3D->initialize();
	camera3D->set_transform({
		CVector3::BASIS,
		Quaternion::EulerDegree(45,0,0),
		{0,10,-10}
		});
	parent = CreateUnique<GameObject>();
	parent->reset_object("Sphere.obj");
	child = CreateUnique<GameObject>();
	child->reset_object("Sphere.obj");
	child->set_parent(parent->get_hierarchy());

	parentCollider = CreateShared<SphereCollider>();
	parentCollider->initialize();
	parentCollider->set_on_collision(
		std::bind(&SceneDemo::on_collision, this, std::placeholders::_1, &parent->get_materials()[0].color)
	);
	parentCollider->set_on_collision_enter(
		std::bind(&SceneDemo::on_collision_enter, this, std::placeholders::_1, &parent->get_materials()[0].color)
	);
	parentCollider->set_on_collision_exit(
		std::bind(&SceneDemo::on_collision_exit, this, std::placeholders::_1, &parent->get_materials()[0].color)
	);
	parentCollider->get_hierarchy().set_parent(parent->get_hierarchy());

	childCollider = CreateShared<SphereCollider>();
	childCollider->initialize();
	childCollider->set_on_collision(
		std::bind(&SceneDemo::on_collision, this, std::placeholders::_1, &child->get_materials()[0].color)
	);
	childCollider->set_on_collision_enter(
		std::bind(&SceneDemo::on_collision_enter, this, std::placeholders::_1, &child->get_materials()[0].color)
	);
	childCollider->set_on_collision_exit(
		std::bind(&SceneDemo::on_collision_exit, this, std::placeholders::_1, &child->get_materials()[0].color)
	);
	childCollider->get_hierarchy().set_parent(child->get_hierarchy());

	singleCollider = CreateShared<SphereCollider>();
	singleCollider->initialize();
#ifdef _DEBUG
	singleCollider->set_on_collision(
		std::bind(&SceneDemo::on_collision, this, std::placeholders::_1, &singleCollider->get_collider_drawer().get_materials()[0].color)
	);
	singleCollider->set_on_collision_enter(
		std::bind(&SceneDemo::on_collision_enter, this, std::placeholders::_1, &singleCollider->get_collider_drawer().get_materials()[0].color)
	);
	singleCollider->set_on_collision_exit(
		std::bind(&SceneDemo::on_collision_exit, this, std::placeholders::_1, &singleCollider->get_collider_drawer().get_materials()[0].color)
	);
#endif // _DEBUG

	collisionManager = CreateUnique<CollisionManager>();
	collisionManager->register_collider("Parent", parentCollider);
	collisionManager->register_collider("Single", singleCollider);
	collisionManager->register_collider("Child", childCollider);
}

void SceneDemo::finalize() {
}

void SceneDemo::begin() {
}

void SceneDemo::update() {
	camera3D->update();
}

void SceneDemo::begin_rendering() {
	camera3D->update_matrix();
	parent->begin_rendering(*camera3D);
	child->begin_rendering(*camera3D);
}

void SceneDemo::late_update() {
	collisionManager->update();
	collisionManager->collision("Parent", "Single");
	collisionManager->collision("Single", "Child");
}

void SceneDemo::draw() const {
	RenderPathManager::BeginFrame();
	parent->draw();
	child->draw();
#ifdef _DEBUG
	collisionManager->debug_draw3d(*camera3D);
#endif // _DEBUG
	RenderPathManager::Next();
}

void SceneDemo::on_collision(const BaseCollider* const, Color* object) {
	*object = { 1.0f,0,0,1.0f };
}

void SceneDemo::on_collision_enter(const BaseCollider* const, Color* object) {
	*object = { 0,1.0f,0,1.0f };
}

void SceneDemo::on_collision_exit(const BaseCollider* const, Color* object) {
	*object = { 1.0f,1.0f,1.0f,1.0f };
}

#ifdef _DEBUG

#include <externals/imgui/imgui.h>
void SceneDemo::debug_update() {
	ImGui::Begin("DemoScene");
	if (ImGui::Button("StackScene")) {
		SceneManager::SetSceneChange(CreateUnique<SceneDemo>(), true);
	}
	if (ImGui::Button("ChangeScene")) {
		SceneManager::SetSceneChange(CreateUnique<SceneDemo>(), false);
	}
	if (ImGui::Button("PopScene")) {
		SceneManager::PopScene();
	}
	ImGui::End();

	ImGui::Begin("Camera3D");
	camera3D->debug_gui();
	ImGui::End();

	ImGui::Begin("Parent");
	parent->debug_gui();
	ImGui::End();

	ImGui::Begin("Child");
	child->debug_gui();
	ImGui::End();

}
#endif // _DEBUG