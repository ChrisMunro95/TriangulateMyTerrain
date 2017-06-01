#include "MyView.hpp"
#include <tygra/FileHelper.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include <algorithm>
#include "Frustum.h"

MyView::
MyView()
{
}

MyView::
~MyView()
{
}

void MyView::
setScene(std::shared_ptr<const SceneModel::Context> scene)
{
    scene_ = scene;
}

void MyView::
toggleShading()
{
    shade_normals_ = !shade_normals_;
}

void MyView::
windowViewWillStart(std::shared_ptr<tygra::Window> window)
{
    assert(scene_ != nullptr);

    GLint compile_status = 0;
    GLint link_status = 0;

    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    std::string vertex_shader_string = tygra::stringFromFile("terrain_vs.glsl");
    const char *vertex_shader_code = vertex_shader_string.c_str();
    glShaderSource(vertex_shader, 1,
                   (const GLchar **) &vertex_shader_code, NULL);
    glCompileShader(vertex_shader);
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &compile_status);
    if (compile_status != GL_TRUE) {
        const int string_length = 1024;
        GLchar log[string_length]= "";
        glGetShaderInfoLog(vertex_shader, string_length, NULL, log);
        std::cerr << log << std::endl;
    }
 
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    std::string fragment_shader_string = tygra::stringFromFile("terrain_fs.glsl");
    const char *fragment_shader_code = fragment_shader_string.c_str();
    glShaderSource(fragment_shader, 1,
                   (const GLchar **) &fragment_shader_code, NULL);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &compile_status);
    if (compile_status != GL_TRUE) {
        const int string_length = 1024;
        GLchar log[string_length]= "";
        glGetShaderInfoLog(fragment_shader, string_length, NULL, log);
        std::cerr << log << std::endl;
    }

    terrain_sp_ = glCreateProgram();
    glAttachShader(terrain_sp_, vertex_shader);
    glDeleteShader(vertex_shader);
    glAttachShader(terrain_sp_, fragment_shader);
    glDeleteShader(fragment_shader);
    glLinkProgram(terrain_sp_);

    glGetProgramiv(terrain_sp_, GL_LINK_STATUS, &link_status);
    if (link_status != GL_TRUE) {
        const int string_length = 1024;
        GLchar log[string_length]= "";
        glGetProgramInfoLog(terrain_sp_, string_length, NULL, log);
        std::cerr << log << std::endl;
    }


    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    vertex_shader_string = tygra::stringFromFile("shapes_vs.glsl");
    vertex_shader_code = vertex_shader_string.c_str();
    glShaderSource(vertex_shader, 1,
        (const GLchar **)&vertex_shader_code, NULL);
    glCompileShader(vertex_shader);
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &compile_status);
    if (compile_status != GL_TRUE) {
        const int string_length = 1024;
        GLchar log[string_length] = "";
        glGetShaderInfoLog(vertex_shader, string_length, NULL, log);
        std::cerr << log << std::endl;
    }

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    fragment_shader_string = tygra::stringFromFile("shapes_fs.glsl");
    fragment_shader_code = fragment_shader_string.c_str();
    glShaderSource(fragment_shader, 1,
        (const GLchar **)&fragment_shader_code, NULL);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &compile_status);
    if (compile_status != GL_TRUE) {
        const int string_length = 1024;
        GLchar log[string_length] = "";
        glGetShaderInfoLog(fragment_shader, string_length, NULL, log);
        std::cerr << log << std::endl;
    }

    shapes_sp_ = glCreateProgram();
    glAttachShader(shapes_sp_, vertex_shader);
    glDeleteShader(vertex_shader);
    glAttachShader(shapes_sp_, fragment_shader);
    glDeleteShader(fragment_shader);
    glLinkProgram(shapes_sp_);

    glGetProgramiv(shapes_sp_, GL_LINK_STATUS, &link_status);
    if (link_status != GL_TRUE) {
        const int string_length = 1024;
        GLchar log[string_length] = "";
        glGetProgramInfoLog(terrain_sp_, string_length, NULL, log);
        std::cerr << log << std::endl;
    }


    glGenVertexArrays(1, &cube_vao_);
    glBindVertexArray(cube_vao_);
    glGenBuffers(1, &cube_vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, cube_vbo_);
    float cube_vertices[] = {
        -0.5f, 0.f, -0.5f,   0.5f, 0.f, -0.5f,   0.5f, 0.f,  0.5f,
        -0.5f, 0.f, -0.5f,   0.5f, 0.f,  0.5f,  -0.5f, 0.f,  0.5f,
        -0.5f, 0.f,  0.5f,   0.5f, 0.f,  0.5f,   0.5f, 1.f,  0.5f,
        -0.5f, 0.f,  0.5f,   0.5f, 1.f,  0.5f,  -0.5f, 1.f,  0.5f,
         0.5f, 0.f,  0.5f,   0.5f, 0.f, -0.5f,   0.5f, 1.f, -0.5f,
         0.5f, 0.f,  0.5f,   0.5f, 1.f, -0.5f,   0.5f, 1.f,  0.5f,
         0.5f, 0.f, -0.5f,  -0.5f, 0.f, -0.5f,  -0.5f, 1.f, -0.5f,
         0.5f, 0.f, -0.5f,  -0.5f, 1.f, -0.5f,   0.5f, 1.f, -0.5f,
        -0.5f, 0.f, -0.5f,  -0.5f, 0.f,  0.5f,  -0.5f, 1.f,  0.5f,
        -0.5f, 0.f, -0.5f,  -0.5f, 1.f,  0.5f,  -0.5f, 1.f, -0.5f,
        -0.5f, 1.f,  0.5f,   0.5f, 1.f,  0.5f,   0.5f, 1.f, -0.5f,
        -0.5f, 1.f,  0.5f,   0.5f, 1.f, -0.5f,  -0.5f, 1.f, -0.5f,
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices),
                 cube_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(kVertexPosition);
    glVertexAttribPointer(kVertexPosition, 3, GL_FLOAT, GL_FALSE,
        sizeof(glm::vec3), TGL_BUFFER_OFFSET(0));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    const auto& bezier_patches = scene_->getTerrainPatches();
    const size_t number_of_patches = bezier_patches.size();

	//Create Grid Here

	const unsigned int meshSizeX = 256;
	const unsigned int meshSizeY = 256;

	myMesh_ = new Mesh(meshSizeX, meshSizeY);
	
	std::vector<glm::vec3> control_points0;

	//extract the control points from the patches,
	//these control point will be needed when the mesh,
	//generates positions

	for (unsigned int y = 0; y < bezier_patches[0].sizeY(); y++){
		for (unsigned int x = 0; x < bezier_patches[0].sizeX(); x++){

			glm::vec3 cp = bezier_patches[0].at(x, y);

			control_points0.push_back(cp);

		}

	}

	std::vector<glm::vec3> control_points1;
	for (unsigned int y = 0; y < bezier_patches[1].sizeY(); y++){
		for (unsigned int x = 0; x < bezier_patches[1].sizeX(); x++){

			glm::vec3 cp = bezier_patches[1].at(x, y);

			control_points1.push_back(cp);

		}

	}

	myMesh_->genPositions(control_points0, control_points1);

	auto normals = myMesh_->getNormalsArray();

	std::vector<glm::vec3> positions = myMesh_->getPositionsArray();
	const auto UVcoords = myMesh_->getUVcoordsArray();

	tygra::Image displace_image = tygra::imageFromPNG(scene_->getTerrainDisplacementMapName());

	auto numVerts = myMesh_->getVertexCount();
	int count = 0;


	for (unsigned int y = 0; y < meshSizeY; y++){
		for (unsigned int x = 0; x < meshSizeX; x++){

			float texelU = UVcoords[count].x * displace_image.width();
			float texelV = UVcoords[count].y * displace_image.height();

			uint8_t displacment = *(uint8_t*)displace_image((int)texelU, (int)texelV);

			if (count < numVerts-1){
				positions[count++] += normals[count] * (float)displacment;
			}

		}
	}

	myMesh_->setPositionsArray(positions);

	myMesh_->normalize();

	myMesh_->applyNoise(3.5f, true);

	myMesh_->normalize();

	myMesh_->smoothNormals();

	normals = myMesh_->getNormalsArray();

    //initialising a terrain VAO
	const int vertex_count = myMesh_->getVertexCount();
    const int element_count = myMesh_->getElementCount();
	const auto& elements = myMesh_->getElementsArray();
	positions = myMesh_->getPositionsArray();

    glGenBuffers(1, &terrain_mesh_.element_vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrain_mesh_.element_vbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        elements.size() * sizeof(unsigned int),
        elements.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    terrain_mesh_.element_count = elements.size();
	
    glGenBuffers(1, &terrain_mesh_.position_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, terrain_mesh_.position_vbo);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec3),
                 positions.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	glGenBuffers(1, &terrain_mesh_.normal_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, terrain_mesh_.normal_vbo);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3),
		normals.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenVertexArrays(1, &terrain_mesh_.vao);
    glBindVertexArray(terrain_mesh_.vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrain_mesh_.element_vbo);
    
	glBindBuffer(GL_ARRAY_BUFFER, terrain_mesh_.position_vbo);
    glEnableVertexAttribArray(kVertexPosition);
    glVertexAttribPointer(kVertexPosition, 3, GL_FLOAT, GL_FALSE,
                          sizeof(glm::vec3), TGL_BUFFER_OFFSET(0));

	glBindBuffer(GL_ARRAY_BUFFER, terrain_mesh_.normal_vbo);
	glEnableVertexAttribArray(kVertexNormal);
	glVertexAttribPointer(kVertexNormal, 3, GL_FLOAT, GL_FALSE,
		sizeof(glm::vec3), TGL_BUFFER_OFFSET(0));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

	frustum_ = new Frustum();

}

void MyView::
windowViewDidReset(std::shared_ptr<tygra::Window> window,
                   int width,
                   int height)
{
    glViewport(0, 0, width, height);
}

void MyView::
windowViewDidStop(std::shared_ptr<tygra::Window> window)
{
    glDeleteProgram(terrain_sp_);
    glDeleteProgram(shapes_sp_);

    glDeleteBuffers(1, &terrain_mesh_.position_vbo);
    glDeleteBuffers(1, &terrain_mesh_.element_vbo);
    glDeleteVertexArrays(1, &terrain_mesh_.vao);

	delete myMesh_;
	delete frustum_;
}

void MyView::
windowViewRender(std::shared_ptr<tygra::Window> window)
{
    assert(scene_ != nullptr);

	//report the previous query results when it completes
	if (total_draw_query_ != 0){
		GLuint query_ready = GL_FALSE;
		glGetQueryObjectuiv(total_draw_query_, GL_QUERY_RESULT_AVAILABLE, &query_ready);
		if (query_ready){
			GLuint query_result;
			glGetQueryObjectuiv(total_draw_query_, GL_QUERY_RESULT, &query_result);
			std::cout << "Draw Time (Microsecs): " << query_result / 1000 << std::endl;
			glDeleteQueries(1, &total_draw_query_);
			total_draw_query_ = 0;
		}
	}

	bool perform_query;
	
	//Start a new query when one is reguested making sure there is one in progress already
	if (trigger_queries_ == false && total_draw_query_ == 0){
		perform_query = true;
		glGenQueries(1, &total_draw_query_);
		glBeginQuery(GL_TIME_ELAPSED, total_draw_query_);
		trigger_queries_ = false;
	}

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    const float aspect_ratio = viewport[2] / (float)viewport[3];

    const auto& camera = scene_->getCamera();
    glm::mat4 projection_xform = glm::perspective(camera.getVerticalFieldOfViewInDegrees(),
                                                  aspect_ratio,
                                                  camera.getNearPlaneDistance(),
                                                  camera.getFarPlaneDistance());
    glm::vec3 camera_pos = camera.getPosition();
    glm::vec3 camera_at = camera.getPosition() + camera.getDirection();
    glm::vec3 world_up{ 0, 1, 0 };
    glm::mat4 view_xform = glm::lookAt(camera_pos, camera_at, world_up);


    /* TODO: you are free to modify any of the drawing code below */

    glClearColor(0.f, 0.f, 0.25f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, shade_normals_ ? GL_FILL : GL_LINE);

    glUseProgram(terrain_sp_);

    GLuint shading_id = glGetUniformLocation(terrain_sp_, "use_normal");
    glUniform1i(shading_id, shade_normals_);

	glm::mat4 world_xform = glm::mat4(1);

    glm::mat4 view_world_xform = view_xform * world_xform;

    GLuint projection_xform_id = glGetUniformLocation(terrain_sp_,
                                                      "projection_xform");
    glUniformMatrix4fv(projection_xform_id, 1, GL_FALSE,
                       glm::value_ptr(projection_xform));

    GLuint view_world_xform_id = glGetUniformLocation(terrain_sp_,
                                                      "view_world_xform");
    glUniformMatrix4fv(view_world_xform_id, 1, GL_FALSE,
                       glm::value_ptr(view_world_xform));

    glBindVertexArray(terrain_mesh_.vao);
    glDrawElements(GL_TRIANGLES, terrain_mesh_.element_count, GL_UNSIGNED_INT, 0);


 //   glEnable(GL_DEPTH_TEST);
 //   glEnable(GL_CULL_FACE);
 //   glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

 //   glUseProgram(shapes_sp_);

 //   projection_xform_id = glGetUniformLocation(shapes_sp_,
 //                                              "projection_xform");
 //   glUniformMatrix4fv(projection_xform_id, 1, GL_FALSE,
 //                      glm::value_ptr(projection_xform));

 //   glBindVertexArray(cube_vao_);

	//int cubeCount = 0;

	///*
	//##########
	//Extrating the frustum planes here wont cull the cubes properly, 
	//However the performance of the application isn't affected that much,

	//it will either draw all the cubes, or none
	//however if the camera is angled a certain it will draw a couple of thousand cubes.
	//##########
	//*/

	//const glm::mat4 projection_view_model_xform = projection_xform * view_world_xform;

	//const auto planes_ = frustum_->extractPlanes(projection_view_model_xform, true);

 //   for (const auto& pos : scene_->getAllShapePositions())
 //   {
 //       world_xform = glm::translate(glm::mat4(1), glm::vec3(pos.x, 64, -pos.y));
 //       view_world_xform = view_xform * world_xform;

	//	/*
	//	#########
	//	If I extract the frustum planes in this loop, the application slows down tremendosly,
	//	as each time the view changes, it needs to extact all the new data, for that draw frame.

	//	But the culling of the cubes works perfectly fine.
	//	#########
	//	*/
	//	//const glm::mat4 projection_view_model_xform = projection_xform * view_world_xform;

	//	//const auto planes_ = frustum_->extractPlanes(projection_view_model_xform, true);

	//	view_world_xform_id = glGetUniformLocation(shapes_sp_,
	//		"view_world_xform");
	//	glUniformMatrix4fv(view_world_xform_id, 1, GL_FALSE,
	//		glm::value_ptr(view_world_xform));

	//	bool intersecting = frustum_->checkIntersection(glm::vec3(pos, 0));

	//	if (intersecting == true){
	//		//inside the frustum so draw the cube
	//		glDrawArrays(GL_TRIANGLES, 0, 36);
	//		cubeCount++;

	//	}

 //   }

	//if (perform_query){
	//	glEndQuery(GL_TIME_ELAPSED);
	//}

	//std::cout << "Cubes being drawn: " << cubeCount << std::endl;

}
