// Fall 2019

#include <glm/ext.hpp>

#include "A4.hpp"
#include "cs488-framework/MathUtils.hpp"

#define PI 3.1415962

void A4_Render(
		// What to render  
		SceneNode * root,

		// Image to write to, set to a given width and height  
		Image & image,

		// Viewing parameters  
		const glm::vec3 & eye,
		const glm::vec3 & view,
		const glm::vec3 & up,
		double fovy,

		// Lighting parameters  
		const glm::vec3 & ambient,
		const std::list<Light *> & lights
) {

  // Fill in raytracing code here...  

  std::cout << "Calling A4_Render(\n" <<
		  "\t" << *root <<
          "\t" << "Image(width:" << image.width() << ", height:" << image.height() << ")\n"
          "\t" << "eye:  " << glm::to_string(eye) << std::endl <<
	  "\t" << "view: " << glm::to_string(view) << std::endl <<
	  "\t" << "up:   " << glm::to_string(up) << std::endl <<
	  "\t" << "fovy: " << fovy << std::endl <<
          "\t" << "ambient: " << glm::to_string(ambient) << std::endl <<
	  "\t" << "lights{" << std::endl;

	for(const Light * light : lights) {
		std::cout << "\t\t" <<  *light << std::endl;
	}
	std::cout << "\t}" << std::endl;
	std::cout <<")" << std::endl;

	size_t h = image.height();
	size_t w = image.width();

	// "size" of pixel is fovy(degrees) / image width
	// view direction and up direction (unit vectors?)
	double focalLength = 1; //this likely shouldn't need to change
	double pixelSize = (glm::tan(degreesToRadians(fovy/2)) * focalLength * 2) / w;
	//normalize
	glm::vec3 viewDir = glm::normalize(view);
	glm::vec3 viewUp = glm::normalize(up);
	glm::vec3 viewCross = glm::cross(viewDir, viewUp);
	std::cout << "Computed cross vector of length: " << glm::length(viewCross) << std::endl;

	for (uint y = 0; y < h; ++y) {
		for (uint x = 0; x < w; ++x) {
			// build ray from eye through pixel location in world space
			//    depends on eye angle and image size/fov
			//    ray = origin + dVector
			//    ***Parametric
			//    Eye coord + t(Pix coord - Eye coord)

			// descend through scene tree doing ray colision?
			//    bounding box colision first
			//    check for ray with const value of axis aligned plane
			//    flat plane -> for all x,z y=const
			//    mesh colision?? triangles? spheres? course notes?
			//    triangle: cross product of edges gets normal (A,B,C,0)
			//    normal gets plane equation (-D = -(Ax + By + Cz))
			//    solve ray equation for x/y/z
			//    substitute into plane, solve for t -> solve for point QED

			// get x,y,z value of intersection, get normal of mesh
			//    compute amount of light from each source with normal
			//    phong lighting?
			//    mat ambient * ambient light +
			//    mat diffuse * (dir to light . normal) * diffuse light +
			//    mat specular * (reflect dir of light . dir to camera)^shininess * specular light

			glm::vec3 pixel = eye 
				+ viewDir*focalLength 
				- viewUp*(-pixelSize*h/2 + pixelSize/2 + pixelSize*y) 
				+ viewCross*(-pixelSize*w/2 + pixelSize/2 + pixelSize*x);

			glm::vec3 c = traceColor(root, ambient, lights, eye, pixel, 4);
			if (c[0] >= 0) {
				image(x,y,0) = c[0];
				image(x,y,1) = c[1];
				image(x,y,2) = c[2];
			} else {
				// Red: 
				image(x,y,0) = (double)x/w;
				// Green: 
				image(x,y,1) = (double)y/h;
				// Blue: 
				image(x,y,2) = (double)0.5;	
			}

		}
		if (y % (h/12) == 0) std::cout << 100*y/h << "%" << std::endl;
	}

	
}

glm::vec3 traceColor(SceneNode * root, const glm::vec3 & ambient, const std::list<Light *> & lights, const glm::vec3 & eye, const glm::vec3 & pixel, int reflectDepth) {
	//collide with scene
	// needs to track intercept point and node collided
	glm::vec3 img;
	SceneNode * collided = NULL;
	glm::vec3 intercept;
	glm::vec3 normal;
	if (root->rayInterceptScene(eye, pixel, collided, intercept, normal)) {
		// find seen colour with intercept and lights
		normal = glm::normalize(normal);
		// big cast material
		PhongMaterial* mat = (PhongMaterial*)(((GeometryNode*)collided)->m_material);

		//reflections?
		glm::vec3 dirToCamera = glm::normalize(eye-intercept);
		glm::vec3 reflect = glm::normalize(-dirToCamera - 2 * glm::dot(-dirToCamera, normal)*normal);
		// got intercept and reflected ray;
		glm::vec3 refKd = mat->get_kd();
		float refFactor = std::min(1.0, mat->get_shine()/100);
		if (reflectDepth > 0 && refFactor > 0.0) {
			glm::vec3 refCol = traceColor(root, ambient, lights, intercept, intercept+reflect, reflectDepth-1);
			
			if (refCol[0] > 0 && refCol[1] > 0 && refCol[2] > 0) {
				refKd = (refFactor)*refCol + (1-refFactor)*refKd;
			}
		}

		// apply ambient light
		img[0] = refKd[0] * ambient[0];
		img[1] = refKd[1] * ambient[1];
		img[2] = refKd[2] * ambient[2];
				
		for(const Light * light : lights) {
			// check for shadow
			SceneNode * dummyC = NULL;
			glm::vec3 dummyI;
			glm::vec3 dummyN;
			if (root->rayInterceptScene(intercept, light->position, dummyC, dummyI, dummyN)){
				// intersected with scene (care for self intersection)
				continue;
			}
			float dLight = glm::length(intercept-light->position);
			glm::vec3 dirToLight = glm::normalize(light->position-intercept);
			for (int col = 0; col < 3; col++) {
				// diffuse
				img[col] += refKd[col] 
				    * glm::dot(dirToLight, normal) 
				    * (light->colour[col]); //falloff?
				
				// specular
				img[col] += mat->get_ks()[col]
				    * pow(glm::dot(reflect, dirToCamera) ,mat->get_shine())
				    * (light->colour[col]);		
			}
		}
		
		// normal map
		/*
		img[0] = normal[0];
		img[1] = normal[1];
		img[2] = normal[2];
		*/
	} else {
		// signal background
		// Red: 
		img[0] = (double)-1.0;
		// Green: 
		img[1] = (double)-1.0;
		// Blue: 
		img[2] = (double)-1.0;
	}
	return img;
}
