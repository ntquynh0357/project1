#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

int main(int argc, char *argv[]) {
    // Nếu không có lệnh nào được truyền vào sau xargs, báo lỗi và thoát
    if (argc < 2) {
        fprintf(2, "Usage: xargs command...\n");
        exit(1);
    }

    // Mảng chứa các đối số sẽ truyền cho hàm exec
    char *xargs_argv[MAXARG];
    int i;
    
    // 1. Sao chép các đối số ban đầu từ argv sang xargs_argv
    // Ví dụ: xargs echo bye -> argv[1] là "echo", argv[2] là "bye"
    for (i = 1; i < argc; i++) {
        xargs_argv[i - 1] = argv[i];
    }

    char buf;
    char line[512]; // Bộ đệm tạm thời để chứa dữ liệu của 1 dòng
    int line_idx = 0;

    // 2. Đọc luồng dữ liệu đầu vào (File Descriptor 0 là Standard Input)
    // Vòng lặp này sẽ đọc từng ký tự (1 byte) một cho đến khi hết dữ liệu
    while (read(0, &buf, 1) == 1) {
        
        // Nếu gặp dấu xuống dòng, nghĩa là đã đọc trọn vẹn 1 dòng
        if (buf == '\n') {
            line[line_idx] = 0; // Chèn ký tự kết thúc chuỗi (\0)

            // 3. Đưa chuỗi vừa đọc được vào vị trí đối số cuối cùng
            xargs_argv[argc - 1] = line;
            xargs_argv[argc] = 0; // Quy tắc của exec: phần tử cuối mảng phải là 0 (NULL)

            // 4. Tạo tiến trình con để chạy lệnh
            if (fork() == 0) {
                // --- Đây là Tiến trình con ---
                // Hàm exec sẽ thay thế tiến trình hiện tại bằng chương trình mới
                exec(xargs_argv[0], xargs_argv);
                
                // Nếu exec chạy thành công, nó sẽ không bao giờ chạy đến dòng dưới này.
                // Nếu chạy đến đây tức là bị lỗi (ví dụ: gõ sai tên lệnh)
                fprintf(2, "exec %s failed\n", xargs_argv[0]);
                exit(1);
            } else {
                // --- Đây là Tiến trình cha ---
                // Tiến trình cha (xargs) phải đợi lệnh của con chạy xong mới đọc dòng tiếp theo
                wait(0);
            }
            
            // Đặt lại chỉ số để chuẩn bị đọc dòng tiếp theo
            line_idx = 0; 
        } else {
            // Nếu không phải ký tự xuống dòng, tiếp tục ghép ký tự vào mảng line
            line[line_idx++] = buf;
        }
    }
    
    exit(0);
}
