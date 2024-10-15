#pragma once

#ifdef _DEBUG

#include <Game/GameScene/Timeline/Timeline.h>

#include <optional>

class GameObject;
class Camera3D;

class TimelineEditor {
	enum EditType {
		Create,
		Edit,
		Delete,
	};

	enum class MouseState {
		None,
		SingleCrick,
		DoubleCrick,
	};

	struct Ray {
		Vector3 origin;
		Vector3 direction;
	};

public:


public:
	void initialize(Timeline* timeline_, Camera3D* camera_);
	void editor_gui();
	void draw_preview();

private:
	void input();
	void export_json_all();
	void demoplay();
	void config();
	void wave_editor();
	void pop_list();
	void get_nearest_enemy();
	void detail_window();
	void operation();

private:
	void reset_wave(std::optional<int> wave);
	void reset_select(std::optional<int> value);
	void run_demoplay();
	void stop_demoplay();
	void sort_all_wave_deta();

private:
	Timeline* timeline;
	Camera3D* camera;

	EditType editType;

	std::optional<Vector3> rayToGround;
	Vector3 rayToGroundDelta;
	MouseState mouseState;
	float mouseLeftCrickTimer;

	std::optional<int> editWave;
	std::optional<int> selectPopData;
	std::optional<int> nearestIndex;

	float forawardAngle;

	std::vector<GameObject> previews;

	bool isDemoplayEditWave;
};

#endif // _DEBUG
