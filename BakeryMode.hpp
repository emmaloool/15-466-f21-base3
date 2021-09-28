#include "Mode.hpp"

#include "Scene.hpp"
#include "Sound.hpp"
#include "Load.hpp"

#include <glm/glm.hpp>

#include "GL.hpp"
#include <vector>
#include <array>
#include <deque>
#include <stack>

struct BakeryMode : Mode {
	BakeryMode();
	virtual ~BakeryMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	//----- game state -----

	//local copy of the game scene (so code can change it during gameplay):
	Scene scene;

	typedef enum FruitKind {
		Cherry = 0,
		Blueberry = 1,
		Banana = 2,
		GreenKiwi = 3,
		YellowKiwi = 4,
		Honeydew = 5,
		Cantaloupe = 6,
		Watermelon = 7,
		WhiteDragonFruit = 8,
		RedDragonFruit = 9
	} FruitType;

	// for debugging
	std::string fruits_names[10] = {"Cherry", "Blueberry", "Banana", "GreenKiwi", "YellowKiwi", "Honeydew", "Cantaloupe", "Watermelon", "WhiteDragonFruit", "RedDragonFruit"};

	// Wrapper for instances of various fruit objects
	struct Fruit {
		std::string name; 		// For display purposes
		FruitType type = Cherry;		// Fruits are initialized as cherries
		bool available = true; 			// Fruits will be marked as unavailable when placed on tart
		bool staged = false;			// Only stage fruits that have been properly setup
		bool ready = false;				// Only throw fruits that are ready!
		Scene::Transform *transform;	// Transform associated with this fruit
		glm::vec3 init_position;		// Useful for preparing fruit/throwing fruit
		glm::vec3 dest_position;		// Final position after throwing fruit onto scene
	};
	std::vector<Fruit> fruits;

	uint8_t current_fruit_index = 0;
	static const uint8_t max_fruit = 10;		// Max amount of fruits in the scene
	uint8_t num_fruit = 0;						// Number of fruits placed
	std::array<bool, max_fruit> seen_fruits; 	// just to make sure all fruits were loaded

	int8_t get_next_available_index();

	// Tart Shell
	struct Tart {
		Scene::Transform *base;
		Scene::Transform *rim;
		Scene::Transform *cream;
	};
	Tart tart;
	float tart_base_depth = 1.0f;
	glm::vec3 hidden_fruit_pos;

	std::array<bool, max_fruit>prev_fruit_presence;
	std::array<bool, max_fruit>fruit_presence;

	// Collisions/throwing constants
	const float collision_delta = 1.5f;
	const float speed = 20.0f;

	// Allow user to undo/redo their throws (for better layering, placement)
	std::stack<uint8_t> placed_fruit_indices;
	
	//camera:
	Scene::Camera *camera = nullptr;

	// ---------------------------- Sounds ----------------------------
	// Looping sounds (set up ambience)
	std::shared_ptr<Sound::PlayingSample> relaxing_loop;
	std::shared_ptr<Sound::PlayingSample> boss_loop;

	static const uint8_t num_insts = 10;
	struct InstructReqs {
		std::vector<Load<Sound::Sample> *> samples;
		std::vector<std::array<int8_t, max_fruit>> requirements;
	};
	InstructReqs instructions;
	uint8_t cur_instr_i = 0;

	enum Phase {
		RECOVERY, 
		INSTRUCT,
		MOVE
	};
	Phase phase = RECOVERY;

	const float RECOVERY_TIME = 7.5f;
	const float INSTRUCT_TIME = 3.0f;
	float MOVE_TIME = 8.0f;

	// Track total elapsed time between iteration phases
	float time = -2.0f;		// start earlier than 0 to test

	// Overall game tracking state
	int8_t score = 0;
	bool game_over = false;

};