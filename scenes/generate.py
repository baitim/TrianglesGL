import random
import os
count_tests = 10

to_curr_dir = os.path.dirname(os.path.realpath(__file__))
scenes_in_dir = to_curr_dir + "/scenes_in"
os.system("mkdir -p " + scenes_in_dir)

for test_num in range(0, count_tests) :
    file_name = scenes_in_dir + "/scene_" + f'{test_num+1:03}' + ".in"
    file = open(file_name, 'w')

    count_triangles = random.randint(250, 300)

    min_x = -2
    max_x =  2
    min_y = -2
    max_y =  2
    min_z = -2
    max_z =  2

    cub_sqrt_count_triangles = (int)(count_triangles ** (1/3))
    stepx  = cub_sqrt_count_triangles
    stepy  = cub_sqrt_count_triangles
    stepz  = cub_sqrt_count_triangles
    
    shiftx = (max_x - min_x) / (stepx + 1) / 2
    shifty = (max_y - min_y) / (stepy + 1) / 2
    shiftz = (max_z - min_z) / (stepz + 1) / 2

    x_0 = (max_x + min_x) / 2
    y_0 = (max_y + min_y) / 2
    z_0 = (max_z + min_z) / 2

    test_str = str(count_triangles) + "\n\n"
    file.write(test_str)
    for j in range(count_triangles) :

        test_str = ""
        cur_shiftx = random.randint(-stepx, stepx) * shiftx
        cur_shifty = random.randint(-stepy, stepy) * shifty
        cur_shiftz = random.randint(-stepz, stepz) * shiftz
        for k in range(3) :
            point_x = random.uniform(-shiftx, shiftx) + cur_shiftx + x_0
            point_y = random.uniform(-shifty, shifty) + cur_shifty + y_0
            point_z = random.uniform(-shiftz, shiftz) + cur_shiftz + z_0

            test_str += f'{point_x:<20}' + " " + f'{point_y:<20}' + " " + f'{point_z:<20}' + "\n"

        test_str += "\n"
        file.write(test_str)

    file.close()
    print("scene ", test_num + 1, " generated")