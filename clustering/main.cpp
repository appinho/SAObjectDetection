#include <opencv2/opencv.hpp>
#include <iostream>
#include <queue>

using namespace cv;
using namespace std;
int main(){

	for(int i = 0; i < 154; i++){

		stringstream ss;
		ss << setw(10) << setfill('0') << i;
		string filename = ss.str();

		string image_filename = "./images/" + filename + ".png";

		Mat image;
	    image = imread( image_filename, 1 );
	    imshow("Display Image", image);
	    waitKey(0);

	    vector<Vec3b> color_clusters;
	    color_clusters.push_back(Vec3b(128, 255,   0));
	    color_clusters.push_back(Vec3b(255, 128,   0));
	    color_clusters.push_back(Vec3b(  0, 255, 128));
	    color_clusters.push_back(Vec3b(  0, 128, 255));
	    color_clusters.push_back(Vec3b(128, 255,   0));
	    color_clusters.push_back(Vec3b(255, 128,   0));
	    color_clusters.push_back(Vec3b(255,   0,   0));
	    color_clusters.push_back(Vec3b(  0, 255,   0));
	    color_clusters.push_back(Vec3b(  0,   0, 255));
	    color_clusters.push_back(Vec3b(255, 255,   0));
	    color_clusters.push_back(Vec3b(  0, 255, 255));
	    color_clusters.push_back(Vec3b(255,   0, 255));


	    Vec3b color_car = Vec3b(142, 0, 0);
	    Vec3b color_ped = Vec3b(60, 20, 220);

	    Mat output = Mat::zeros(image.rows, image.cols, CV_8UC3);

	    vector< vector<Point> > clusters;
	    int counter = -1;
	    // Occlusion: Jump only over closer pixel
	    int kernel = 1;
	    int min_points_per_cluster = 150;
	    time_t startt = clock();
	    for(int x = 0; x < image.rows; x++){
	    	for(int y = 0; y < image.cols; y++){
	    		if((image.at<Vec3b>(x,y) == color_car || image.at<Vec3b>(x,y) == color_ped) && 
	    			output.at<Vec3b>(x,y) == Vec3b(0,0,0)){
	    			counter++;
	    			int counter_cluster = 0;
	    			Vec3b color_output = color_clusters[counter % color_clusters.size()];
	    			queue<Point> neighbor_queue;
					neighbor_queue.push(Point(x,y));
					vector<Point> cluster_points;

					// cout << "Start " << x << " " << y << " " << image.at<Vec3b>(x,y) << endl;
					while(!neighbor_queue.empty()){
						counter_cluster++;
						int c_x = neighbor_queue.front().x;
						int c_y = neighbor_queue.front().y;
						cluster_points.push_back(neighbor_queue.front());
						neighbor_queue.pop();
	    				output.at<Vec3b>(c_x,c_y) = color_output;
	    				// cout << "Add " << c_x << " " << c_y << " " << image.at<Vec3b>(c_x,c_y) << endl;
						for(int k = -kernel; k <= kernel; ++k){
							for(int l = -kernel; l <= kernel; ++l){

								// Dont check the center cell itself
								if(k == 0 && l == 0) continue;

								// Calculate neighbor cell indices
								int n_x = c_x + k;
								int n_y = c_y + l;

								// Check if neighbor cell is out of bounce
								if(n_x >= 0 && n_x < image.rows &&
									n_y >= 0 && n_y < image.cols){

									Vec3b color_neighbor = image.at<Vec3b>(n_x,n_y);
	    							// cout << n_x << " " << n_y << " " 
	    							// 	<< (color_neighbor == image.at<Vec3b>(x,y)) << " " 
	    							// 	<< output.at<Vec3b>(n_x,n_y)
	    							// 	<< endl;

									if(color_neighbor == image.at<Vec3b>(x,y) && 
										output.at<Vec3b>(n_x,n_y) == Vec3b(0,0,0)){


										output.at<Vec3b>(n_x,n_y) = color_output;
										neighbor_queue.push(Point(n_x,n_y));
									}
								}
							}
						}
					}
					clusters.push_back(cluster_points);
	    		}
	    	}	
	    }
        cerr << "Done! time : " << (double)(clock()-startt)/CLOCKS_PER_SEC << "s." << endl;

	    // cout << "Found " << counter << " Clusters" << endl;
	    counter = 0;
	    // output = Mat::zeros(image.rows, image.cols, CV_8UC3);
	    for(int c = 0; c < clusters.size(); c++){
	    	// min points dependent on average depth
			if(clusters[c].size() > min_points_per_cluster){
				counter++;
				Rect bounding_box = boundingRect(clusters[c]);
				// cout << bounding_box.x << " " << bounding_box.y <<
				//  " " << bounding_box.width << " " << bounding_box.height << endl;
				Point top_left = Point(bounding_box.y, bounding_box.x);
				Point bot_right = Point(
					bounding_box.y + bounding_box.height,
					bounding_box.x + bounding_box.width);
				Vec3b color = color_clusters[c % color_clusters.size()];
				int linewidth = 4;
				rectangle(output, top_left, bot_right, color, linewidth, 4);
			}
			else{
				for(int rev = 0; rev < clusters[c].size(); rev++){
					int x = clusters[c][rev].x;
					int y = clusters[c][rev].y;
					output.at<Vec3b>(x,y) = Vec3b(0,0,0);
				}
			}

	    }

		// cout << "Cluster " << counter << " has " << counter_cluster << " pixels!" << endl;
	    cout << "Found " << counter << " Clusters" << endl;
	    imshow("Output", output);
	    waitKey(0);
	}
    return -1;
}