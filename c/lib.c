
struct point {
    int x;
    int y;
    int z;
    char* name;
};

struct point pt = (struct point) {1,2,3,"hello"};
struct point pt2 = (struct point) {1,2,3,"goodbye"};

int distance(struct point pt) {
    return pt.x + pt.y + pt.z;
}