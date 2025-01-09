#include "RubiksCube.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>




RubiksCube::RubiksCube() 
    : locks{false, false, false, false, false, false} , Sensitivity(1.0f) , RotationAngle(90) , RotationDirection(1)   { // Initialize all faces to true
    generateSmallCubes();  // Automatically generate small cubes on creation
}

RubiksCube::~RubiksCube() {
    for (SmallCube* cube : smallCubes) {
        delete cube;
    }
}

void RubiksCube::generateSmallCubes() {
    cubesPosition.clear(); 
    cubesPosition.resize(3, std::vector<std::vector<SmallCube*>>(3, std::vector<SmallCube*>(3)));


    smallCubes.clear();

    int index = 0;
    const int offset = 1; // Offset to handle negative indices

    // Resize the cubesPosition vector to 3x3x3

    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            for (int z = -1; z <= 1; ++z) {
                glm::vec3 position(x, y, z); 
                SmallCube* smallCube = new SmallCube (position, index);
                
                // Update the model matrix for the cube
                glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), position);
                smallCube->setModelMatrix(modelMatrix);

                smallCubes.push_back(smallCube);

                // Use offset to index into cubesPosition
                cubesPosition[x + offset][y + offset][z + offset] = smallCube;

                if (index == 13) { 
                    this->centerCube = smallCube ; 
                }
                index++;
            }
        }
    }
}


std::vector<SmallCube*> RubiksCube::getSmallCubes(){

return this->smallCubes ; 

}



   
void RubiksCube::render(Shader& shader, VertexArray& va, IndexBuffer& ib, glm::mat4 proj, glm::mat4 view) {
    for (SmallCube* cube : smallCubes) {
        // Apply transformations (position, rotation) to each small cube


        glm::mat4 model = cube->getModelMatrix() ;    
        

       glm::mat4 mvp =  proj * view * model ;

        glm::vec4 color = glm::vec4(
             1.0f,  // Red based on x position
             1.0f,  // Green based on y position
             1.0f,  // Blue based on z position
            1.0f             // Alpha
        );

        // Set uniform variables for the shader
        shader.Bind();
        shader.SetUniformMat4f("u_MVP", mvp);
        shader.SetUniform1i("u_Texture", 0);
        shader.SetUniform4f("u_Color", color);

        // Render the small cube  
        va.Bind();
        ib.Bind();
        GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
        shader.Unbind();
    }
}



void RubiksCube::rotateRightWall() {
    std::cout << "Rotating Right Wall by " << (RotationAngle * RotationDirection) << " degrees\n";

    // Get the position of the centerCube to keep the wall's rotation relative to it
    glm::vec3 centerPosition = this->centerCube->getPosition();
    glm::vec3 rightWallCenter = centerPosition + glm::vec3(-1.0f, 0.0f, 0.0f); // Assuming right wall is at x = center - 1

    for (int i = 0; i < smallCubes.size(); ++i) {
        SmallCube* cube = smallCubes[i];

        // Check if the cube is part of the right wall (relative to the centerCube)
        if (cube->getPosition().x == centerPosition.x - 1) { // Right of centerCube
            // Debug: Original position
            glm::vec3 originalPos = cube->getPosition();
            std::cout << "Original Position: " << originalPos.x << ", " << originalPos.y << ", " << originalPos.z << "\n";

            // Perform rotation around the correct axis
            glm::mat4 translationToOrigin = glm::translate(glm::mat4(1.0f), -rightWallCenter);
            glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians((float)RotationAngle * RotationDirection), glm::vec3(1.0f, 0.0f, 0.0f));
            glm::mat4 translateBack = glm::translate(glm::mat4(1.0f), +rightWallCenter);

            glm::mat4 finalMatrix = translateBack * rotationMatrix * translationToOrigin;
            cube->setModelMatrix(finalMatrix * cube->getModelMatrix());

            // Debug: New position
            std::cout << "New Position: " << cube->getPosition().x << ", " << cube->getPosition().y << ", " << cube->getPosition().z << "\n";
        }
    }

    // Locking condition check
    if (RotationAngle == std::abs(45)) {
        this->locks[0] = !this->locks[0];
    }
}



void RubiksCube::rotateLeftWall(){

        std::cout << "Rotating Left Wall by " << (RotationAngle * RotationDirection) << " degrees\n";

    // Get the position of the centerCube to keep the wall's rotation relative to it
    glm::vec3 centerPosition = this->centerCube->getPosition();

    for (int i = 0; i < smallCubes.size(); ++i) {
        SmallCube* cube = smallCubes[i];
        
        // Check if the cube is part of the right wall (relative to the centerCube)
        if (cube->getPosition().x == centerPosition.x + 1) { 

            glm::vec3 originalPos = cube->getPosition();
            std::cout << "Original Position: " << originalPos.x << ", " << originalPos.y << ", " << originalPos.z << "\n";

        

              glm::vec3 position = this->getPosition() ;    
                    

            
            glm::mat4 translationToOrigin = glm::translate(glm::mat4(1.0f), - position); // Translate by (-x1, -y1, -z1)
            glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians((float)RotationAngle * RotationDirection), glm::vec3(1.0f, 0.0f, 0.0f));
            glm::mat4 translateBack = glm::translate(glm::mat4(1.0f), + position); 

            glm::mat4 finalMatrix =  translateBack *  rotationMatrix    * translationToOrigin;  
            cube->setModelMatrix(finalMatrix * cube->getModelMatrix());
          
            // Debug: New position
            std::cout << "New Position: " << cube->getPosition().x << ", " << cube->getPosition().y << ", " << cube->getPosition().z << "\n";
        }
    }

     if (RotationAngle == std::abs(45)) {
        this->locks[1] = !this->locks[1];
    }
}



void RubiksCube::rotateUpWall() {
    std::cout << "Rotating Up Wall by " << (RotationAngle * RotationDirection) << " degrees\n";

        // Get the position of the centerCube to keep the wall's rotation relative to it
    glm::vec3 centerPosition = this->centerCube->getPosition();

    for (int i = 0; i < smallCubes.size(); ++i) {
        SmallCube* cube = smallCubes[i];
        
        // Check if the cube is part of the down wall (relative to the centerCube)
        if (cube->getPosition().z == centerPosition.z + 1) { // Downward from centerCube (considering -1)
            // Debug: Original position
      

              glm::vec3 position = this->getPosition() ;    

            glm::mat4 translationToOrigin = glm::translate(glm::mat4(1.0f), - position); 
            glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians((float)RotationAngle * RotationDirection), glm::vec3(0.0f, 0.0f, 1.0f));
            glm::mat4 translateBack = glm::translate(glm::mat4(1.0f), + position);


            glm::mat4 finalMatrix =  translateBack *  rotationMatrix    * translationToOrigin;  
            cube->setModelMatrix(finalMatrix * cube->getModelMatrix());

            // Debug: New position
            std::cout << "New Position: " << cube->getPosition().x << ", " << cube->getPosition().y << ", " << cube->getPosition().z << "\n";
        }
    }

    // Locking condition check
    if (RotationAngle == std::abs(45)) {
        this->locks[2] = !this->locks[2];
    }
}


void RubiksCube::rotateDownWall() {
    std::cout << "Rotating Down Wall by " << (RotationAngle * RotationDirection) << " degrees\n";

    // Get the position of the centerCube to keep the wall's rotation relative to it
    glm::vec3 centerPosition = this->centerCube->getPosition();

    for (int i = 0; i < smallCubes.size(); ++i) {
        SmallCube* cube = smallCubes[i];
        
        // Check if the cube is part of the down wall (relative to the centerCube)
        if (cube->getPosition().z == centerPosition.z - 1) { // Downward from centerCube (considering -1)
            // Debug: Original position


              glm::vec3 position = this->getPosition() ;    

            glm::mat4 translationToOrigin = glm::translate(glm::mat4(1.0f), - position); 
            glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians((float)RotationAngle * RotationDirection), glm::vec3(0.0f, 0.0f, 1.0f));
            glm::mat4 translateBack = glm::translate(glm::mat4(1.0f), + position); 

            glm::mat4 finalMatrix =  translateBack *  rotationMatrix    * translationToOrigin;  
            cube->setModelMatrix(finalMatrix * cube->getModelMatrix());

            // Debug: New position
            std::cout << "New Position: " << cube->getPosition().x << ", " << cube->getPosition().y << ", " << cube->getPosition().z << "\n";
        }
    }

    // Locking condition check
    if (RotationAngle == std::abs(45)) {
        this->locks[3] = !this->locks[3];
    }
}

void RubiksCube::rotateBackWall() {
    std::cout << "Rotating Back Wall by " << (RotationAngle * RotationDirection) << " degrees\n";

    // Get the position of the centerCube to keep the wall's rotation relative to it
    glm::vec3 centerPosition = this->centerCube->getPosition();

    for (int i = 0; i < smallCubes.size(); ++i) {
        SmallCube* cube = smallCubes[i];
        
        // Check if the cube is part of the back wall (relative to the centerCube)
        if (cube->getPosition().y == centerPosition.y - 1) { // Behind the centerCube (considering -1)
            // Debug: Original position
            glm::vec3 originalPos = cube->getPosition();
            std::cout << "Original Position: " << originalPos.x << ", " << originalPos.y << ", " << originalPos.z << "\n";

              glm::vec3 position = this->getPosition() ;    
                           
            glm::mat4 translationToOrigin = glm::translate(glm::mat4(1.0f), - position); 
            glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians((float)RotationAngle * RotationDirection), glm::vec3(0.0f, 1.0f, 0.0f));
            glm::mat4 translateBack = glm::translate(glm::mat4(1.0f), + position); 

            
            glm::mat4 finalMatrix =  translateBack *  rotationMatrix    * translationToOrigin;  
            cube->setModelMatrix(finalMatrix * cube->getModelMatrix());


            // Debug: New position
            std::cout << "New Position: " << cube->getPosition().x << ", " << cube->getPosition().y << ", " << cube->getPosition().z << "\n";
        }
    }

    // Locking condition check
    if (RotationAngle == std::abs(45)) {
        this->locks[4] = !this->locks[4];
    }
}


void RubiksCube::rotateFrontWall() {
    std::cout << "Rotating Front Wall by " << (RotationAngle * RotationDirection) << " degrees\n";

    // Get the position of the centerCube to keep the wall's rotation relative to it
    glm::vec3 centerPosition = this->centerCube->getPosition();

    for (int i = 0; i < smallCubes.size(); ++i) {
        SmallCube* cube = smallCubes[i];
        
        // Check if the cube is part of the front wall (relative to the centerCube)
        if (cube->getPosition().y == centerPosition.y + 1) { // In front of the centerCube (considering +1)
            // Debug: Original position
            glm::vec3 originalPos = cube->getPosition();
            std::cout << "Original Position: " << originalPos.x << ", " << originalPos.y << ", " << originalPos.z << "\n";

           
              glm::vec3 position = this->getPosition() ;    
                       
            glm::mat4 translationToOrigin = glm::translate(glm::mat4(1.0f), - position); 
            glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians((float)RotationAngle * RotationDirection), glm::vec3(0.0f, 1.0f, 0.0f));
            glm::mat4 translateBack = glm::translate(glm::mat4(1.0f), + position); 



            
            glm::mat4 finalMatrix =  translateBack *  rotationMatrix    * translationToOrigin;  
            cube->setModelMatrix(finalMatrix * cube->getModelMatrix());

            // Debug: New position
            std::cout << "New Position: " << cube->getPosition().x << ", " << cube->getPosition().y << ", " << cube->getPosition().z << "\n";
        }
    }

    // Locking condition check
    if (RotationAngle == std::abs(45)) {
        this->locks[5] = !this->locks[5];
    }
}




// Locks : 

bool RubiksCube::canRotateRightWall() {
    return !(locks[2] || locks[3] || locks[4] || locks[5]); // All must be false
}

bool RubiksCube::canRotateLeftWall() {
    return !(locks[2] || locks[3] || locks[4] || locks[5]); // Same condition as RightWall
}

bool RubiksCube::canRotateUpWall() {
    return !(locks[0] || locks[1] || locks[4] || locks[5]); // All must be false
}

bool RubiksCube::canRotateDownWall() {
    return !(locks[0] || locks[1] || locks[4] || locks[5]); // Fix inconsistent condition
}

bool RubiksCube::canRotateBackWall() {
    return !(locks[0] || locks[1] || locks[2] || locks[3]); // All must be false
}

bool RubiksCube::canRotateFrontWall() {
    return !(locks[0] || locks[1] || locks[2] || locks[3]); // All must be false
}


// Arrows pressed : 


void RubiksCube::UpArrow() {

    for (SmallCube* cube : this->smallCubes) {                   
        // Create a translation matrix for moving up
        glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -  this->Sensitivity, 0.0f));

        // Apply the translation to the model matrix
        glm::mat4 updatedModelMatrix = translationMatrix * cube->getModelMatrix();

        // Set the updated model matrix
        cube->setModelMatrix(updatedModelMatrix);
    }
}



void RubiksCube::DownArrow(){   


     for (SmallCube* cube : this->smallCubes) {                   
        // Create a translation matrix for moving up
        glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f,  this->Sensitivity, 0.0f));
        std::cout << "Sensitivity value: " << this->Sensitivity << std::endl;

        // Apply the translation to the model matrix
        glm::mat4 updatedModelMatrix = translationMatrix * cube->getModelMatrix();

        // Set the updated model matrix
        cube->setModelMatrix(updatedModelMatrix);
    }
}


void RubiksCube::RightArrow(){

    for (SmallCube* cube : this->smallCubes) {                   
        // Create a translation matrix for moving up
        glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-(this->Sensitivity), 0.0f ,  0.0f));
        std::cout << "Sensitivity value: " << this->Sensitivity << std::endl;

        // Apply the translation to the model matrix
        glm::mat4 updatedModelMatrix = translationMatrix * cube->getModelMatrix();

        // Set the updated model matrix
        cube->setModelMatrix(updatedModelMatrix);

    }
}

glm::vec3 RubiksCube::getPosition(){

    return this->cubesPosition[1][1][1]->position ; 
}




void RubiksCube::LeftArrow(){

       for (SmallCube* cube : this->smallCubes) {                   
        // Create a translation matrix for moving up
        glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3((this->Sensitivity), 0.0f ,  0.0f));
        std::cout << "Sensitivity value: " << this->Sensitivity << std::endl;

        // Apply the translation to the model matrix
        glm::mat4 updatedModelMatrix = translationMatrix * cube->getModelMatrix();

        // Set the updated model matrix
        cube->setModelMatrix(updatedModelMatrix);

    }

     
}
        // Set the updated matrix
   










