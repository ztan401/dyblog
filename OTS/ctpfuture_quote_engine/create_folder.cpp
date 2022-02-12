//
// Created by 观鱼 on 2022/2/8.
//
#include "create_folder.h"
#include <dirent.h>
#include <sys/io.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <string>

void create_folder(const std::string &dir_path) {
    if (access(dir_path.c_str(), 0) == -1) {        //判断该文件夹是否存在
        int flag = mkdir(dir_path.c_str(), S_IRWXU);//Linux创建文件夹
        if (flag == 0) {                            //创建成功
            std::cout << "Create directory successfully." << std::endl;
        } else {//创建失败
            std::cout << "Fail to create directory." << std::endl;
            throw std::exception();
        }
    } else {
        std::cout << "This directory already exists." << std::endl;
    }
}
