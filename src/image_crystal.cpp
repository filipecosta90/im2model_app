#include "image_crystal.hpp"
#include <iostream>

Image_Crystal::Image_Crystal(){

}

/** getters **/

/** setters **/
bool Image_Crystal::set_experimental_image_path(std::string path){
    std::cout << "setting experimental path: " << path << std::endl;
    experimental_image_path = path;
    return true;
}

