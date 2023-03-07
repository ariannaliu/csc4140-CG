#include "Triangle.hpp"
#include "rasterizer.hpp"
#include <eigen3/Eigen/Eigen>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <cmath>
// add some other header files you need

constexpr double MY_PI = 3.1415926;

Eigen::Matrix4f get_view_matrix(Eigen::Vector3f eye_pos)
{
    Eigen::Matrix4f view = Eigen::Matrix4f::Identity();

    Eigen::Matrix4f translate;
    translate << 1, 0, 0, -eye_pos[0], 0, 1, 0, -eye_pos[1], 0, 0, 1,
        -eye_pos[2], 0, 0, 0, 1;

    view = translate * view;
    // std::clog << "view" << std::endl << view << std::endl;  // check data

    return view;
}


Eigen::Matrix4f get_model_matrix(float rotation_angle, Eigen::Vector3f T, Eigen::Vector3f S, Eigen::Vector3f P0, Eigen::Vector3f P1)
{

    //Step 1: Build the Translation Matrix M_trans:
    Eigen::Matrix4f M_trans;
    M_trans << 1,0,0,T[0],
               0,1,0,T[1],
               0,0,1,T[2],
               0,0,0,1;

    //Step 2: Build the Scale Matrix S_trans:
    Eigen::Matrix4f S_trans;
    S_trans << S[0],0,0,0,
               0,S[1],0,0,
               0,0,S[2],0,
               0,0,0,1;

    //Step 3: Implement Rodrigues' Rotation Formular, rotation by angle theta around axix u, then get the model matrix
	// The axis u is determined by two points, u = P1-P0: Eigen::Vector3f P0 ,Eigen::Vector3f P1  
    // Create the model matrix for rotating the triangle around a given axis. // Hint: normalize axis first
    Eigen::Matrix4f P_trans;
    Eigen::Vector3f PToOrigin = P1 - P0;
    // Normalize vector 
    Eigen::Vector3f P_axis;
    for(int i=0; i<3; i++){
        P_axis[i] = PToOrigin[i]/sqrt(pow(PToOrigin[0],2) + pow(PToOrigin[1],2) + pow(PToOrigin[2],2));
    }
    float ux = P_axis[0];
    float uy = P_axis[1];
    float uz = P_axis[2];
    float angle = rotation_angle/180 * MY_PI;
    P_trans << cos(angle) + pow(ux,2)*(1-cos(angle)), ux*uy*(1-cos(angle))-uz*sin(angle),ux*uz*(1-cos(angle))+uy*sin(angle),0,
               uy*ux*(1-cos(angle))+uz*sin(angle), cos(angle) + pow(uy,2)*(1-cos(angle)), uy*uz*(1-cos(angle))-ux*sin(angle),0,
               uz*ux*(1-cos(angle))-uy*sin(angle), uz*uy*(1-cos(angle))+ux*sin(angle), cos(angle) + pow(uz,2)*(1-cos(angle)),0,
               0,0,0,1;

    Eigen::Matrix4f model = S_trans*P_trans*M_trans;
    std::cout << "my model is \n" << model << std::endl;
	//Step 4: Use Eigen's "AngleAxisf" to verify your Rotation
	Eigen::AngleAxisf rotation_vector(angle, Vector3f(P_axis[0], P_axis[1], P_axis[2]));  
	// Eigen::Matrix3f rotation_matrix;
	Eigen::Matrix3f rotation_matrix = rotation_vector.toRotationMatrix();
    std::cout << "Eigen model is \n" << rotation_matrix << std::endl;

	return model;
}



Eigen::Matrix4f get_projection_matrix(float eye_fov, float aspect_ratio,
                                      float zNear, float zFar)
{
    // Implement this function
    Eigen::Matrix4f projection;

    eye_fov = eye_fov / 180 * MY_PI;
    float top = zNear * tan(eye_fov * 0.5);
    float bottom = -top;
    float right = top * aspect_ratio;
    float left = -right;

    Eigen::Matrix4f ortho; 
    ortho <<    2 / (right - left), 0, 0, (right + left) / (right - left),
                0, 2 / (top - bottom), 0, (top + bottom) / (top - bottom),
                0, 0, 2 / (zNear - zFar), (zFar + zNear) / (zFar - zNear),
                0, 0, 0, 1;
    Eigen::Matrix4f PerstToOrtho; 
    PerstToOrtho << zNear, 0, 0, 0,
                    0, zNear, 0, 0,
                    0, 0, zNear + zFar, -zNear * zFar,
                    0, 0, 1, 0;
    projection = ortho * PerstToOrtho;
    // std::clog << "projection" << std::endl << projection << std::endl; //check

    return projection;
}

int main(int argc, const char** argv)
{
    float angle = 0;
    bool command_line = false;
    std::string filename = "result.png";

    if (argc >= 3) {
        command_line = true;
        angle = std::stof(argv[2]); // -r by default
        if (argc == 4) {
            filename = std::string(argv[3]);
        }
        else
            return 0;
    }

    rst::rasterizer r(1024, 1024);
    // define your eye position "eye_pos" to a proper position
    Eigen::Vector3f eye_pos = {0, 0, 5};

    // define a triangle named by "pos" and "ind"
    std::vector<Eigen::Vector3f> pos{{2, 0, -2}, {0, 2, -2}, {-2, 0, -2}};

    std::vector<Eigen::Vector3i> ind{{0, 1, 2}};


    auto pos_id = r.load_positions(pos);
    auto ind_id = r.load_indices(ind);

    int key = 0;
    int frame_count = 0;

    // added parameters for get_projection_matrix(float eye_fov, float aspect_ratio,float zNear, float zFar)
    float eye_fov = 45;
    float aspect_ratio = 1;
    float zNear = 0.1;
    float zFar = 50;
    Eigen::Vector3f axis(1, 0, 0);
    Eigen::Vector3f T = {0,0,0};
    Eigen::Vector3f S = {1,1,1};
    Eigen::Vector3f P1 = {3,0,1};
    Eigen::Vector3f P0 = {0,-2,0};

    if (command_line) {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        r.set_model(get_model_matrix(angle, T, S, P0, P1));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(eye_fov, aspect_ratio, zNear, zFar));

        r.draw(pos_id, ind_id, rst::Primitive::Triangle);
        cv::Mat image(1024, 1024, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);

        cv::imwrite(filename, image);

        return 0;
    }

    while (key != 27) {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        r.set_model(get_model_matrix(angle, T, S, P0, P1));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(eye_fov, aspect_ratio, zNear, zFar));

        r.draw(pos_id, ind_id, rst::Primitive::Triangle);

        cv::Mat image(1024, 1024, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);
        cv::imshow("image", image);
        key = cv::waitKey(10);

        std::cout << "frame count: " << frame_count++ << '\n';
        std::clog << "angle: " << angle << std::endl;
    

        if (key == 'a') {
            angle += 10;
        }
        else if (key == 'd') {
            angle -= 10;
        }
    }

    return 0;
}
