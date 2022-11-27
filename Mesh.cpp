//
// Created by cccccci on 2022/11/24.
//
#include "Mesh.h"

#define MAX_BUFFER_SIZE 128

Mesh::Mesh() {
    
}
void Mesh::load(const char* filename )
{

    // Add your code here.
    //load the obj file
    std::ifstream ifile;
    ifile.open(filename,std::ios::in);
    char buffer[MAX_BUFFER_SIZE];
    while(ifile.getline(buffer,MAX_BUFFER_SIZE)){
        //change char to stringstream
        std::stringstream ss(buffer);
        //the first character
        std::string s;
        ss >> s;
        if("v" == s){
            //3D float vector
            Vector3f v;
            //store the values separately
            ss >> v[0] >> v[1] >> v[2];
            vecv.push_back(v);
        } else if("vn" == s){
            Vector3f vn;
            ss >> vn[0] >> vn[1] >> vn[2];
            vecn.push_back(vn);
        } else if("vt" == s){
            Vector2f vt;
            ss >> vt[0] >> vt[1];
            vect.push_back(vt);
        } else if("f" == s){
            std::vector<unsigned> vf;
            //separate face data by /
            char separator = '/';
            int n = 0;
            for(int i = 0;i<3;i++){
                ss >> s;
                //replace the separator to space
                replace(s.begin(),s.end(),separator,' ');
                //use the new ss to change data type
                std::stringstream ss2(s);
                while(ss2 >> n){
                    //store abcdefg as number
                    vf.push_back(n);
                }
            }
            vecf.push_back(vf);
        }
    }
    //std::cout<<xmax<<"," << xmin << "," <<ymax<<","<<ymin<<std::endl;
}
void Mesh::draw(){
    for(int i = 0;i< vecf.size();i++){
        if(vecf[i].size() == 6){
            unsigned v1 = vecf[i][0];
            unsigned t1 = vecf[i][1];
            unsigned v2 = vecf[i][2];
            unsigned t2 = vecf[i][3];
            unsigned v3 = vecf[i][4];
            unsigned t3 = vecf[i][5];
            //compute the normal by taking the cross product of the edges
            Vector3f edge1 = vecv[v2-1]-vecv[v1-1];
            Vector3f edge2 = vecv[v3-1]-vecv[v1-1];
            Vector3f normal = Vector3f::cross(edge1,edge2);
            glBegin(GL_TRIANGLES);
            glNormal3d(normal.x(),normal.y(),normal.z());
            glVertex3d(vecv[v1-1][0], vecv[v1-1][1], vecv[v1-1][2]);
            glNormal3d(normal.x(),normal.y(),normal.z());
            glVertex3d(vecv[v2-1][0], vecv[v2-1][1], vecv[v2-1][2]);
            glNormal3d(normal.x(),normal.y(),normal.z());
            glVertex3d(vecv[v3-1][0], vecv[v3-1][1], vecv[v3-1][2]);
            glEnd();
        }
        if(vecf[i].size() == 9){
            unsigned v1 = vecf[i][0];
            unsigned n1 = vecf[i][2];
            unsigned v2 = vecf[i][3];
            unsigned n2 = vecf[i][5];
            unsigned v3 = vecf[i][6];
            unsigned n3 = vecf[i][8];
            glBegin(GL_TRIANGLE_STRIP);
            glNormal3d(vecn[n1-1][0], vecn[n1-1][1], vecn[n1-1][2]);
            glVertex3d(vecv[v1-1][0], vecv[v1-1][1], vecv[v1-1][2]);
            glNormal3d(vecn[n2-1][0], vecn[n2-1][1], vecn[n2-1][2]);
            glVertex3d(vecv[v2-1][0], vecv[v2-1][1], vecv[v2-1][2]);
            glNormal3d(vecn[n3-1][0], vecn[n3-1][1], vecn[n3-1][2]);
            glVertex3d(vecv[v3-1][0], vecv[v3-1][1], vecv[v3-1][2]);
            glEnd();
        }

        

    }
}

