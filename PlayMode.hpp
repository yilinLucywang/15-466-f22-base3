#include "Mode.hpp"

#include "Scene.hpp"
#include "Sound.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>

struct PlayMode : Mode {
	PlayMode();
	virtual ~PlayMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	//----- game state -----

	//input tracking:
	struct Button {
		uint8_t downs = 0;
		uint8_t pressed = 0;
	} left, right, down, up, w_left, w_right, w_down, w_up, space;

	//local copy of the game scene (so code can change it during gameplay):
	Scene scene;

	//music coming from the tip of the leg (as a demonstration):
	std::shared_ptr< Sound::PlayingSample > hint_one_shot;
	std::shared_ptr< Sound::PlayingSample > leg_tip_loop;
	Scene::Transform *player = nullptr;
	Scene::Transform *box0 = nullptr;
	Scene::Transform *box1 = nullptr;
	Scene::Transform *box2 = nullptr;
	Scene::Transform *box3 = nullptr;
	Scene::Transform *box4 = nullptr;
	Scene::Transform *box5 = nullptr;
	Scene::Transform *box6 = nullptr;
	Scene::Transform *box7 = nullptr;
	Scene::Transform *box8 = nullptr;
	Scene::Transform *box9 = nullptr;
	Scene::Transform *box10 = nullptr;
	Scene::Transform *box11 = nullptr;
	Scene::Transform *star = nullptr;
	std::vector<Scene::Transform*> box_vector = {};
	int collision_box();
	void gen_hint_sound();
	void hide_star();
	int star_index = 1;
	int score = 0;
	//camera:
	Scene::Camera *camera = nullptr;

};