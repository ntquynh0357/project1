#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h" // Chứa định nghĩa DIRSIZ và struct dirent

// Hàm phụ trợ: Lấy tên file cuối cùng từ một đường dẫn dài
// Ví dụ: truyền vào "a/aa/b" -> trả về "b"
char* basename(char *path) {
    char *p;
    // Tìm ký tự '/' cuối cùng trong chuỗi
    for(p = path + strlen(path); p >= path && *p != '/'; p--)
        ;
    p++;
    return p;
}

// Hàm chính xử lý đệ quy
void tree(char *path, int depth) {
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    // 1. Cố gắng mở file/thư mục
    if((fd = open(path, 0)) < 0){
        fprintf(2, "tree: cannot open %s\n", path);
        // Đề bài yêu cầu: "If the directory cannot be opened: print error and exit"
        if(depth == 0) exit(1); 
        return;
    }

    // 2. Lấy thông tin (stat) để xem nó là T_FILE hay T_DIR
    if(fstat(fd, &st) < 0){
        fprintf(2, "tree: cannot stat %s\n", path);
        close(fd);
        return; // Lỗi stat một mục con thì in lỗi rồi return để chạy tiếp mục khác
    }

    // 3. In ra khoảng trắng để thụt lề dựa trên độ sâu (depth)
    for(int i = 0; i < depth; i++){
        printf("  "); 
    }

    // Lấy tên gốc của file/thư mục để in ra
    char *name = basename(path);

    // 4. Kiểm tra loại của mục hiện tại
    if(st.type == T_DIR) {
        printf("%s/\n", name); // Thư mục thì in thêm dấu /
    } else {
        printf("%s\n", name);  // File thường thì in nguyên tên
    }

    // 5. Nếu là thư mục, tiến hành đọc các file con bên trong nó
    if(st.type == T_DIR) {
        // Kiểm tra xem đường dẫn có bị quá dài so với bộ đệm không
        if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
            printf("tree: path too long\n");
            close(fd);
            return;
        }
        
        // Chuẩn bị đường dẫn mới: copy "path" vào "buf", thêm dấu "/"
        strcpy(buf, path);
        p = buf + strlen(buf);
        *p++ = '/';

        // Vòng lặp đọc từng nội dung trong thư mục
        while(read(fd, &de, sizeof(de)) == sizeof(de)){
            // Bỏ qua các mục trống (đã bị xóa)
            if(de.inum == 0)
                continue;
            
            // QUAN TRỌNG: Phải bỏ qua thư mục "." và ".." để không bị đệ quy vô hạn
            if(strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
                continue;

            // Ghép tên file con vào đường dẫn
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0; 
            
            // Gọi đệ quy cho file/thư mục con, tăng độ sâu (depth) lên 1
            tree(buf, depth + 1);
        }
    }
    
    close(fd);
}

int main(int argc, char *argv[]) {
    if(argc > 2) {
        fprintf(2, "Usage: tree [directory]\n");
        exit(1);
    }

    // Nếu người dùng nhập "tree a", argv[1] là "a"
    // Nếu chỉ nhập "tree", mặc định duyệt thư mục hiện tại "."
    if(argc == 2) {
        tree(argv[1], 0);
    } else {
        tree(".", 0);
    }

    exit(0);
}
