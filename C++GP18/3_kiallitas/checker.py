
def move_tuple(tup, direction):
    if direction == '<':
        return (tup[0], tup[1] - 1)
    elif direction == '>':
        return (tup[0], tup[1] + 1)
    elif direction == '^':
        return (tup[0] - 1, tup[1])
    elif direction == 'v':
        return (tup[0] + 1, tup[1])
    
    return None

def is_valid(coord, _max):
    return 0 <= coord[0] and coord[0] < _max[0] and \
            0 <= coord[1] and coord[1] < _max[1]

def run_custom_checker(t_obj, r_obj):
    r_obj.result = False;
    r_obj.score = 0.0;
    r_obj.message = "Wrong solution";
    
    max_coord = None
    field = []
    with open(t_obj.testcase_input_path) as input_file:
        first_line = True
        for line in input_file:
            if first_line:
                _max = line.strip().split(' ')
                first_line = False
                max_coord = (int(_max[0]), int(_max[1]))
            elif line:
                field.append(list(line.strip()))
    
    with open(t_obj.testcase_output_path) as output_file:
        first_line = True
        line_count = 0
        for line in output_file:
            if first_line:
                first_line = False
                if not line.strip().isdigit():
                    r_obj.message = "Wrong output format"
                    break
                line_count = int(line.strip())
                continue
            
            if line_count == 0:
                break
            
            line_count -= 1
            
            data = line.strip().split(' ')
            if len(data) != 3 or not data[0].isdigit() or not data[1].isdigit() \
                or not len(data[2]) == 1 or not data[2] in "<>^v":
                r_obj.message = "Wrong output format"
                break
            
            coord = (int(data[0])-1, int(data[1])-1)
            if not is_valid(coord, max_coord) or not field[coord[0]][coord[1]].isdigit():
                r_obj.message = "Wrong output data"
                break
            
            count = int(field[coord[0]][coord[1]]);
            
            field[coord[0]][coord[1]] = "+"
            
            coord = move_tuple(coord, data[2])
            while count > 0 and is_valid(coord, max_coord):
                if field[coord[0]][coord[1]] == '.':
                    count -= 1
                    field[coord[0]][coord[1]] = '+'
                elif field[coord[0]][coord[1]] == 'E':
                    r_obj.result = True;
                    r_obj.score = 1.0;
                    r_obj.message = "Success";
                    break
                coord = move_tuple(coord, data[2])
            else:
                continue
            break
        else:
            if line_count > 0:
                    r_obj.message = "Wrong output format"

class Dummy:
    pass

import sys
if __name__ == "__main__":
    if len(sys.argv) < 3:
        print "python",sys.argv[0],"<input path> <output path>"
    else:
        t_obj = Dummy()
        setattr(t_obj, "testcase_input_path", sys.argv[1])
        setattr(t_obj, "testcase_output_path", sys.argv[2])
        
        r_obj = Dummy()
        setattr(r_obj, "result", False)
        setattr(r_obj, "score", 0.0)
        setattr(r_obj, "message", "Please set")
        run_custom_checker(t_obj, r_obj)
        
        print r_obj.result, r_obj.score, r_obj.message