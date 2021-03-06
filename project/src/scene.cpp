
#include "scene.hpp"
#include <math.h>   
#include <cmath>

float extern pi;

float extern disc_radius;
float extern turn_radius;
float extern racetrack_length;
float extern racetrack_thickness;

float extern barrier_height;
float extern barrier_radius1;
int extern post_resolution;
int extern turn_resolution;

float extern departure_height;
float extern departure_thickness;
float extern departure_radius;

float extern tube_height;
float extern tube_thickness;
float extern arc_thickness;

float extern pilar_height;
float extern pilar_thickness;
float extern pivot_height; 

#include "kart.h"

using namespace cgp;

void scene_structure::initialize()
{
	// Basic set-up
	// global_frame.initialize(mesh_primitive_frame(), "Frame");
	environment.camera.axis = camera_spherical_coordinates_axis::z;
	environment.camera.look_at({ 2.0f,-4.0f,2.0f }, { 0,0,0 });
	global_frame.initialize(mesh_primitive_frame(), "Frame");
	//environment.camera.position_camera = { disc_radius/2.0, 0.6 * racetrack_length/2.0, 5.0f };
	//environment.camera.manipulator_rotate_roll_pitch_yaw(-Pi/2.0, Pi/2.0, Pi/2.0);
	
	// mesh const terrain_mesh = create_terrain_mesh();
	// terrain.initialize(terrain_mesh, "terrain");
	// terrain.shading.color = { 0.6f,0.85f,0.5f };
	skybox.initialize("assets/Gu_RainbowRoad/","skybox");
	racetrack = create_racetrack_mesh_drawable();

	kart = create_kart();
	//test();

	// Key 3D positions
	buffer<vec3> key_positions = { {disc_radius/2.0f,racetrack_length/2.0f,0}, {disc_radius/2.0f,racetrack_length/2.0f,0}, {disc_radius/2.0f,racetrack_length,0}, {0,racetrack_length + disc_radius/2.0f,0}, {-disc_radius/2.0f,racetrack_length,0}, {-disc_radius/2.0f,racetrack_length/2.0f,0}, {-disc_radius/2.0f,0,0}, {0,-disc_radius/2.0f,0}, {disc_radius/2,0,0}, {disc_radius/2.0f,racetrack_length/2.0f,0}, {disc_radius/2.0f,racetrack_length/2.0f,0} };
	key_derivee = {{0,racetrack_length,0},{0,racetrack_length,0},{0,racetrack_length/3.0,0},{-racetrack_length/2.0,0,0},{0,racetrack_length,0},{0,racetrack_length,0},{0,-1,0},{1,0,0},{0,1,0},{0,1,0},{0,1,0} };
	// Key times (time at which the position must pass in the corresponding position)
	buffer<float> key_times = { 0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f };

	// Initialize the helping structure to display/interact with these positions
	keyframe.initialize(key_positions, key_times);

	int N = key_times.size();
	timer.t_min = key_times[1];
	timer.t_max = key_times[N - 2];
	timer.t = timer.t_min;
	

	//int idx = find_index_of_interval(2.0, key_times);
}

void scene_structure::update_camera()
{
	

	// The camera moves forward all the time
	//   We consider in this example a constant velocity, so the displacement is: velocity * dt * front-camera-vector
	float const dt = timer.update();
	//vec3 const forward_displacement = 0;
	//camera.position_camera += forward_displacement;
	// camera.position_camera += (dt, dt, dt);
	//camera.manipulator_rotate_trackball({ 0,0 }, { dt,dt });
	float compteur = 0;
	float t = timer.t;
	vec3 p = interpolation(avancementKart, keyframe.key_positions, keyframe.key_times);
	vec3 p_p = derivee_interpolation(avancementKart, keyframe.key_positions, keyframe.key_times);
	vec3 posCamera =p -  p_p / norm(p_p) * 8 + vec3(-0.5,0,3);
	vec3 regardCamera = p + vec3(-0.5,0,0);
	environment.camera.look_at(posCamera, p);
	// The camera rotates if we press on the arrow keys
	//  The rotation is only applied to the roll and pitch degrees of freedom.
	//float const pitch = 0.5f; // speed of the pitch
	//float const roll = 0.7f; // speed of the roll
	//if (keyboard.up)
	//	camera.position_camera += {0,0.1,0};
	//if (keyboard.down)
	//	camera.position_camera -= {0,0.1,0};
	//if (keyboard.right)
	//	camera.manipulator_rotate_roll_pitch_yaw(roll * dt, 0, 0);
	//if (keyboard.left)
	//	camera.manipulator_rotate_roll_pitch_yaw(-roll * dt, 0, 0);
}



void scene_structure::display()
{
	// Basic elements of the scene
	
	inputs_keyboard_parameters const& keyboard = inputs.keyboard;
	environment.light = environment.camera.position();
	draw(skybox, environment);
	if (gui.display_frame)
		draw(global_frame, environment);
	
	//draw(terrain,environment);
	
	//float racetrack_incline = M_PI/12.0;
	//racetrack["racetrack1"].transform.translation = {0,0,evaluate_terrain_height(0, 0) + pilar_height};
	//racetrack["racetrack_pivot1"].transform.rotation = cgp::rotation_transform::from_axis_angle({1,0,0}, -racetrack_incline);
	//racetrack["racetrack1"].transform.rotation = cgp::rotation_transform::from_axis_angle({1,0,0}, racetrack_incline);

	racetrack.update_local_to_global_coordinates();
	draw(racetrack, environment);
	
	// Update the current time
	timer.update();
	float t = timer.t;

	// clear trajectory when the timer restart
	//if (t < timer.t_min + 0.1f)
	//	keyframe.trajectory.clear();

	// Compute the interpolated position
	//  This is this function that you need to complete
	//vec3 p = interpolation_hermite(t, keyframe.key_positions, keyframe.key_times, key_derivee);
	//vec3 p_p = derivee_hermite(t, keyframe.key_positions, keyframe.key_times,key_derivee);
	if (keyboard.up)
	{
		avancementKart += 0.01;
	}
	if (keyboard.down)
	{
		avancementKart -= 0.01;
	}
	faireAvancerKart(avancementKart, kart, timer.t_min, timer.t_max, keyframe);
	kart.update_local_to_global_coordinates();
	draw(kart, environment);
	keyframe.display_current_position(interpolation(avancementKart, keyframe.key_positions, keyframe.key_times), environment);


	
}


void scene_structure::display_gui()
{
	//ImGui::Checkbox("Frame", &gui.display_frame);
	//ImGui::SliderFloat("Time scale", &timer.scale, 0.0f, 2.0f);
	//ImGui::SliderFloat("Particle emission", &timer.event_period, 0.1f, 2.0f);
}


