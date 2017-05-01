//
// Created by julie on 13/02/17.
//

#include "Sprite_Renderer.h"


Sprite_Renderer::Sprite_Renderer(const Shader &shader, GLfloat repeat){
    this->shader = shader;
    this->initRenderData(repeat);
}

Sprite_Renderer::Sprite_Renderer(const Shader &shader){
    this->shader = shader;
    this->initRenderData();
}


Sprite_Renderer::~Sprite_Renderer(){
    glDeleteVertexArrays(1, &this->quadVAO);
}

// SKYBOX
void Sprite_Renderer::initRenderData()
{
    // Configure VAO/VBO
    GLuint VBO;

    glGenVertexArrays(1, &this->quadVAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(this->quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    GLfloat skyboxVertices[] = {
            // Positions
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            -1.0f,  1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f,  1.0f
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);

    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

// SQUARE
void Sprite_Renderer::initRenderData(GLfloat repeat)
{
    // Configure VAO/VBO
    GLuint VBO;

    glGenVertexArrays(1, &this->quadVAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(this->quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    GLfloat vertices2d[] = {
            // Pos      // Tex
            0.0f, 0.0f, 0.0f, repeat,
            0.0f, 1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, repeat, repeat,
            1.0f, 1.0f, repeat, 0.0f
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2d), &vertices2d, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

// TEXTURED 3D SQUARE
void Sprite_Renderer::DrawSprite(State_Manager &manager, const Tex &texture, GLboolean &destroy, glm::vec3 position, glm::vec2 size, glm::vec3 rotation, GLboolean billboard, glm::vec2 deplace, glm::mat4 projection, glm::mat4 view)
{
    // Prepare transformations
    manager.Active(this->shader);
    glm::mat4 model;
    model = glm::translate(model, position);  // First translate (transformations are: scale happens first, then rotation and then final translation happens; reversed order)

    model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f)); //to rotate around center
    model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f,0.0f,0.0f)); // Then rotate
    model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f,1.0f,0.0f));
    model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f,0.0f,1.0f));
    model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));

    model = glm::scale(model, glm::vec3(size, 0)); // Last scale

    this->shader.SetMatrix4("model", model);
    this->shader.SetMatrix4("view", view);
    this->shader.SetMatrix4("projection", projection);

    this->shader.SetVector2f("deplace", deplace);
    if(billboard) {
        this->shader.SetInteger("isBillboard", 0);
        this->shader.SetVector2f("Billboard_Size", size);
        if(!Camera::in_frustrum_square(projection*view*model))
            destroy = GL_TRUE;

    }

    glActiveTexture(GL_TEXTURE0);
    manager.ActiveTex2D(texture);

    glBindVertexArray(this->quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
    this->shader.SetVector2f("deplace", glm::vec2(0));
    if(billboard)
        this->shader.SetInteger("isBillboard", 0);
}

// COLORED SQUARE
void Sprite_Renderer::DrawSprite(State_Manager &manager, glm::vec2 position, glm::vec2 size, GLboolean isCircle, glm::vec3 color1, glm::vec3 color2, Effect effect, glm::mat4 projection, glm::mat4 view, GLboolean border)
{
    //Prepare transformations
    manager.Active(this->shader);
    glm::mat4 model;
    model = glm::translate(model, glm::vec3(position,0.0f));

    model = glm::scale(model, glm::vec3(size, 0.0f));

    this->shader.SetMatrix4("model", model);
    this->shader.SetMatrix4("view", view);
    this->shader.SetMatrix4("projection", projection);

    this->shader.SetInteger("border", border);
    this->shader.SetInteger("isCircle", isCircle);
    this->shader.SetInteger("effect", effect);
    this->shader.SetVector3f("color1", color1);
    this->shader.SetVector3f("color2", color2);

    glBindVertexArray(this->quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
    this->shader.SetInteger("effect", -1);
}

// SKYBOX
void Sprite_Renderer::DrawSprite(State_Manager &manager, const Tex &texture, glm::mat4 projection, glm::mat4 view)
{
    // Prepare transformations
    manager.Active(this->shader);

    glDepthFunc(GL_LEQUAL);
    view = glm::mat4(glm::mat3(view));
    this->shader.SetMatrix4("view", view);
    this->shader.SetMatrix4("projection", projection);

    glBindVertexArray(this->quadVAO);
    glActiveTexture(GL_TEXTURE0);
    manager.ActiveTex3D(texture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    glDepthFunc(GL_LESS);
}