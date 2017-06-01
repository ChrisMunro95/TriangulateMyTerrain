#pragma once

#include <SceneModel/Context.hpp>
#include <tygra/WindowViewDelegate.hpp>
#include <tgl/tgl.h>
#include <glm/glm.hpp>
#include "Mesh.h"
#include "Frustum.h"

class MyView : public tygra::WindowViewDelegate
{
public:

    MyView();

    ~MyView();

    void
    setScene(std::shared_ptr<const SceneModel::Context> scene);

    void
    toggleShading();

private:

    void
    windowViewWillStart(std::shared_ptr<tygra::Window> window) override;

    void
    windowViewDidReset(std::shared_ptr<tygra::Window> window,
                       int width,
                       int height) override;

    void
    windowViewDidStop(std::shared_ptr<tygra::Window> window) override;

    void
    windowViewRender(std::shared_ptr<tygra::Window> window) override;

private:

    std::shared_ptr<const SceneModel::Context> scene_;
	Mesh *myMesh_{ nullptr };
	Frustum *frustum_{nullptr};

    GLuint terrain_sp_{ 0 };
    GLuint shapes_sp_{ 0 };

    bool shade_normals_{ false };

    struct MeshGL
    {
        GLuint position_vbo{ 0 };
		GLuint normal_vbo{ 0 };
        GLuint element_vbo{ 0 };
        GLuint vao{ 0 };
        int element_count{ 0 };
    };
    MeshGL terrain_mesh_;

	GLuint total_draw_query_ = 0;
	bool trigger_queries_ = false;

    enum
    {
        kVertexPosition = 0,
        kVertexNormal = 1,
    };

	GLuint cube_vao_{ 0 };
	GLuint cube_vbo_{ 0 };

};
