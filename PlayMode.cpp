#include "PlayMode.hpp"

#include "LitColorTextureProgram.hpp"

#include "DrawLines.hpp"
#include "Mesh.hpp"
#include "Load.hpp"
#include "gl_errors.hpp"
#include "data_path.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <random>
#include <cmath>

GLuint hexapod_meshes_for_lit_color_texture_program = 0;
Load< MeshBuffer > hexapod_meshes(LoadTagDefault, []() -> MeshBuffer const * {
	MeshBuffer const *ret = new MeshBuffer(data_path("game3Sound.pnct"));
	hexapod_meshes_for_lit_color_texture_program = ret->make_vao_for_program(lit_color_texture_program->program);
	return ret;
});

Load< Scene > hexapod_scene(LoadTagDefault, []() -> Scene const * {
	return new Scene(data_path("game3Sound.scene"), [&](Scene &scene, Scene::Transform *transform, std::string const &mesh_name){
		Mesh const &mesh = hexapod_meshes->lookup(mesh_name);

		scene.drawables.emplace_back(transform);
		Scene::Drawable &drawable = scene.drawables.back();

		drawable.pipeline = lit_color_texture_program_pipeline;

		drawable.pipeline.vao = hexapod_meshes_for_lit_color_texture_program;
		drawable.pipeline.type = mesh.type;
		drawable.pipeline.start = mesh.start;
		drawable.pipeline.count = mesh.count;

	});
});

Load< Sound::Sample > background_sample(LoadTagDefault, []() -> Sound::Sample const * {
	return new Sound::Sample(data_path("background.wav"));
});

Load< Sound::Sample > hint_sample(LoadTagDefault, []() -> Sound::Sample const * {
	return new Sound::Sample(data_path("bell_clue.wav"));
});


PlayMode::PlayMode() : scene(*hexapod_scene) {
	//get pointers to leg for convenience:
	for (auto &transform : scene.transforms) {
		if(transform.name == "Icosphere"){
			player = &transform;
		}

		if(transform.name == "Cube0"){
			box0 = &transform;
		}
		if(transform.name == "Cube1"){
			box1 = &transform;
		}

		if(transform.name == "Cube2"){
			box2 = &transform;
		}
		if(transform.name == "Cube3"){
			box3 = &transform;
		}

		if(transform.name == "Cube4"){
			box4 = &transform;
		}
		if(transform.name == "Cube5"){
			box5 = &transform;
		}

		if(transform.name == "Cube6"){
			box6 = &transform;
		}
		if(transform.name == "Cube7"){
			box7 = &transform;
		}

		if(transform.name == "Cube8"){
			box8 = &transform;
		}
		if(transform.name == "Cube9"){
			box9 = &transform;
		}

		if(transform.name == "Cube10"){
			box10 = &transform;
		}
		if(transform.name == "Cube11"){
			box11 = &transform;
		}

		if(transform.name == "star"){
			star = &transform;
		}
	}
	// 	if (transform.name == "Hip.FL") hip = &transform;
	// 	else if (transform.name == "UpperLeg.FL") upper_leg = &transform;
	// 	else if (transform.name == "LowerLeg.FL") lower_leg = &transform;
	// }
	// if (hip == nullptr) throw std::runtime_error("Hip not found.");
	// if (upper_leg == nullptr) throw std::runtime_error("Upper leg not found.");
	// if (lower_leg == nullptr) throw std::runtime_error("Lower leg not found.");

	// hip_base_rotation = hip->rotation;
	// upper_leg_base_rotation = upper_leg->rotation;
	// lower_leg_base_rotation = lower_leg->rotation;

	//get pointer to camera for convenience:
	if (scene.cameras.size() != 1) throw std::runtime_error("Expecting scene to have exactly one camera, but it has " + std::to_string(scene.cameras.size()));
	camera = &scene.cameras.front();

	//start music loop playing:
	// (note: position will be over-ridden in update())
	hint_one_shot = Sound::play(*hint_sample, 1.0f, 0.0f);
	leg_tip_loop = Sound::loop_3D(*background_sample, 1.0f, glm::vec3(0.0f, 0.0f, 0.0f), 10.0f);
	box_vector.push_back(box0);
	box_vector.push_back(box1);
	box_vector.push_back(box2);
	box_vector.push_back(box3);
	box_vector.push_back(box4);
	box_vector.push_back(box5);
	box_vector.push_back(box6);
	box_vector.push_back(box7);
	box_vector.push_back(box8);
	box_vector.push_back(box9);
	box_vector.push_back(box10);
	box_vector.push_back(box11);

	//TODO: hide the star
	// std::mt19937 generator (123);
	// std::uniform_real_distribution<double> dis(0.0, 1.0);//source: https://stackoverflow.com/questions/22923551/generating-number-0-1-using-mersenne-twister-c
	// int box_index = floor(dis(generator) * 11.0f);
	// std::cout << box_index << std::endl;
	// std::cout << box_vector[box_index] << std::endl;
	// star->position = box_vector[0]->position; 


}

PlayMode::~PlayMode() {
}


void PlayMode::gen_hint_sound(){
	float x_diff = (star->position.x - player->position.x); 
	float y_diff = (star->position.y - player->position.y);
	bool is_left = true; 
	bool is_front = true;
	if(x_diff < 0){
		is_left = false;
	}
	if(y_diff < 0){
		is_front = true; 
	}

	if(is_left && is_front){
		std::cout << "hello, check position" << std::endl;
	}
	// std::cout << "hello, check position" << std::endl;
	// std::cout << is_left << std::endl;
	// std::cout << is_front << std::endl;

}

int PlayMode::collision_box(){
	for(int i = 0; i < box_vector.size(); i++){
		float x_s = (box_vector[i]->position.x - player->position.x) * (box_vector[i]->position.x - player->position.x);
		float y_s = (box_vector[i]->position.y - player->position.y) * (box_vector[i]->position.y - player->position.y);
		float z_s = (box_vector[i]->position.z - player->position.z) * (box_vector[i]->position.z - player->position.z);
		float dist = sqrt(x_s + y_s + z_s);
		if(dist <= 1.3f){
			std::cout << i << std::endl;
			return i; 
		}
	}
	std::cout << "-1" << std::endl;
	return -1; 
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {

	if (evt.type == SDL_KEYDOWN) {
		if (evt.key.keysym.sym == SDLK_ESCAPE) {
			SDL_SetRelativeMouseMode(SDL_FALSE);
			return true;
		} else if (evt.key.keysym.sym == SDLK_a) {
			left.downs += 1;
			left.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_d) {
			right.downs += 1;
			right.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_w) {
			up.downs += 1;
			up.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_s) {
			down.downs += 1;
			down.pressed = true;
			return true;
		}


		//TODO: key code
		else if (evt.key.keysym.sym == SDLK_LEFT) {
			w_left.downs += 1;
			w_left.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_RIGHT) {
			w_right.downs += 1;
			w_right.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_UP) {
			w_up.downs += 1;
			w_up.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_DOWN) {
			w_down.downs += 1;
			w_down.pressed = true;
			return true;
		}
		//TODO: key code end



	} else if (evt.type == SDL_KEYUP) {
		if (evt.key.keysym.sym == SDLK_a) {
			left.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_d) {
			right.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_w) {
			up.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_s) {
			down.pressed = false;
			return true;
		}

		//TODO: key code
		else if (evt.key.keysym.sym == SDLK_LEFT) {
			w_left.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_RIGHT) {
			w_right.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_UP) {
			w_up.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_DOWN) {
			w_down.pressed = false;
			return true;
		}
		//TODO: key code end

	} else if (evt.type == SDL_MOUSEBUTTONDOWN) {
		if (SDL_GetRelativeMouseMode() == SDL_FALSE) {
			SDL_SetRelativeMouseMode(SDL_TRUE);
			return true;
		}
	} else if (evt.type == SDL_MOUSEMOTION) {
		if (SDL_GetRelativeMouseMode() == SDL_TRUE) {
			glm::vec2 motion = glm::vec2(
				evt.motion.xrel / float(window_size.y),
				-evt.motion.yrel / float(window_size.y)
			);
			camera->transform->rotation = glm::normalize(
				camera->transform->rotation
				* glm::angleAxis(-motion.x * camera->fovy, glm::vec3(0.0f, 1.0f, 0.0f))
				* glm::angleAxis(motion.y * camera->fovy, glm::vec3(1.0f, 0.0f, 0.0f))
			);
			return true;
		}
	}

	return false;
}

void PlayMode::update(float elapsed) {

	gen_hint_sound(); 
	// std::cout << "hello, 111" << std::endl;
	// std::cout << box_vector.size() << std::endl;
	// std::cout << box_vector[0]->position.x << std::endl;
	// for(int i = 0; i < box_vector.size(); i++){
	// 	std::cout << box_vector[i]->position.x << std::endl;
	// }
	// std::cout << "hello, 222" << std::endl;
	collision_box();
	//move sound to follow leg tip position:
	hint_one_shot->set_position(glm::vec3(0.0f, 0.0f, 0.0f), 1.0f / 60.0f);

	//move camera:
	{

		//combine inputs into a move:
		constexpr float PlayerSpeed = 30.0f;
		glm::vec2 move = glm::vec2(0.0f);
		if (left.pressed && !right.pressed) move.x =-1.0f;
		if (!left.pressed && right.pressed) move.x = 1.0f;
		if (down.pressed && !up.pressed) move.y =-1.0f;
		if (!down.pressed && up.pressed) move.y = 1.0f;

		//make it so that moving diagonally doesn't go faster:
		if (move != glm::vec2(0.0f)) move = glm::normalize(move) * PlayerSpeed * elapsed;

		glm::mat4x3 frame = camera->transform->make_local_to_parent();
		glm::vec3 frame_right = frame[0];
		//glm::vec3 up = frame[1];
		glm::vec3 frame_forward = -frame[2];

		camera->transform->position += move.x * frame_right + move.y * frame_forward;
	}

	{ //update listener to camera position:
		glm::mat4x3 frame = camera->transform->make_local_to_parent();
		glm::vec3 frame_right = frame[0];
		glm::vec3 frame_at = frame[3];
		Sound::listener.set_position_right(frame_at, frame_right, 1.0f / 60.0f);
	}



	//move the water bucket
	{
		constexpr float PlayerSpeed = 30.0f;
		glm::vec2 w_move = glm::vec2(0.0f);
		if (w_left.pressed && !w_right.pressed) w_move.x =-1.0f;
		if (!w_left.pressed && w_right.pressed) w_move.x = 1.0f;
		if (w_down.pressed && !w_up.pressed) w_move.y =-1.0f;
		if (!w_down.pressed && w_up.pressed) w_move.y = 1.0f;

		//make it so that moving diagonally doesn't go faster:
		if (w_move != glm::vec2(0.0f)) w_move = glm::normalize(w_move) * PlayerSpeed * elapsed;
		// glm::mat4x3 frame = camera->transform->make_local_to_parent();
		// glm::vec3 frame_right = frame[0];
		// //glm::vec3 up = frame[1];
		// glm::vec3 frame_forward = -frame[2];

		// camera->transform->position += move.x * frame_right + move.y * frame_forward;
		glm::mat4x3 w_frame = player->make_local_to_parent();
		//glm::vec3 w_frame_right = w_frame[2];
		glm::vec3 w_frame_up = w_frame[0];
		glm::vec3 w_frame_forward = w_frame[1];
		//TODO: water bucket move
		player->position += w_move.x * w_frame_up;
		//TODO: issues here
		player->position += w_move.y * w_frame_forward; 
	}

	//reset button press counters:
	left.downs = 0;
	right.downs = 0;
	up.downs = 0;
	down.downs = 0;
}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	//update camera aspect ratio for drawable:
	camera->aspect = float(drawable_size.x) / float(drawable_size.y);

	//set up light type and position for lit_color_texture_program:
	// TODO: consider using the Light(s) in the scene to do this
	glUseProgram(lit_color_texture_program->program);
	glUniform1i(lit_color_texture_program->LIGHT_TYPE_int, 1);
	glUniform3fv(lit_color_texture_program->LIGHT_DIRECTION_vec3, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f,-1.0f)));
	glUniform3fv(lit_color_texture_program->LIGHT_ENERGY_vec3, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 0.95f)));
	glUseProgram(0);

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClearDepth(1.0f); //1.0 is actually the default value to clear the depth buffer to, but FYI you can change it.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS); //this is the default depth comparison function, but FYI you can change it.

	scene.draw(*camera);

	{ //use DrawLines to overlay some text:
		glDisable(GL_DEPTH_TEST);
		float aspect = float(drawable_size.x) / float(drawable_size.y);
		DrawLines lines(glm::mat4(
			1.0f / aspect, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		));

		constexpr float H = 0.09f;
		lines.draw_text("Mouse motion rotates camera; WASD moves; escape ungrabs mouse",
			glm::vec3(-aspect + 0.1f * H, -1.0 + 0.1f * H, 0.0),
			glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
			glm::u8vec4(0x00, 0x00, 0x00, 0x00));
		float ofs = 2.0f / drawable_size.y;
		lines.draw_text("Mouse motion rotates camera; WASD moves; escape ungrabs mouse",
			glm::vec3(-aspect + 0.1f * H + ofs, -1.0 + + 0.1f * H + ofs, 0.0),
			glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
			glm::u8vec4(0xff, 0xff, 0xff, 0x00));
	}
	GL_ERRORS();
}

// glm::vec3 PlayMode::get_leg_tip_position() {
// 	//the vertex position here was read from the model in blender:
// 	return lower_leg->make_local_to_world() * glm::vec4(-1.26137f, -11.861f, 0.0f, 1.0f);
// 	}