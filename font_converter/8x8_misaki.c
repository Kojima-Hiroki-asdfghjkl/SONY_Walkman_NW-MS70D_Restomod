#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define TOTAL_SIZE (65536 * 8)

int main() {
    const char *bdf_path = "C:\\Users\\kojim\\Documents\\GitHub\\SONY_Walkman_NW-MS70D_Restomod\\MisakiGothic2ndRegular8-8.bdf";
    const char *out_bin = "misaki_utf16_8_out.bin";
    const char *out_pbm = "misaki_utf16_8_out.pbm";

    printf("Allocating memory and initializing with 0xFF...\n");
    uint8_t *rom_data = (uint8_t *)malloc(TOTAL_SIZE);
    if (!rom_data) {
        perror("Memory allocation failed");
        return 1;
    }
    // 未定義領域は 0xFF（PBMで見ると白）で初期化
    memset(rom_data, 0xFF, TOTAL_SIZE);

    FILE *f = fopen(bdf_path, "r");
    if (!f) {
        perror("Failed to open BDF file");
        free(rom_data);
        return 1;
    }

    char line[256];
    int current_encoding = -1;
    int in_bitmap = 0;
    int bitmap_row = 0;
    uint8_t raw_bytes[16];
    int char_count = 0;

    int bbx_w = 8, bbx_h = 8, bbx_x_off = 0, bbx_y_off = 0;

    printf("Parsing BDF file with corrected Y-axis mapping...\n");
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\r\n")] = 0;

        if (strncmp(line, "ENCODING", 8) == 0) {
            sscanf(line, "ENCODING %d", &current_encoding);
        } else if (strncmp(line, "BBX", 3) == 0) {
            sscanf(line, "BBX %d %d %d %d", &bbx_w, &bbx_h, &bbx_x_off, &bbx_y_off);
        } else if (strcmp(line, "BITMAP") == 0) {
            in_bitmap = 1;
            bitmap_row = 0;
            memset(raw_bytes, 0, sizeof(raw_bytes));
        } else if (strcmp(line, "ENDCHAR") == 0) {
            in_bitmap = 0;
            if (current_encoding >= 0 && current_encoding < 65536 && bitmap_row > 0) {
                int offset = current_encoding * 8;
                uint8_t final_char_bytes[8];
                memset(final_char_bytes, 0x00, 8);
                
                // 上下反転を直したマッピング計算
                // r = 0 が一番上の行。y_off を考慮して配置する
                for (int r = 0; r < bitmap_row && r < 8; r++) {
                    // y_offに合わせて縦位置をシフト（反転を解消）
                    int dest_row = r - bbx_y_off; 
                    
                    if (dest_row >= 0 && dest_row < 8) {
                        uint32_t val = raw_bytes[r];
                        // Xオフセットの適用
                        if (bbx_x_off > 0) val <<= bbx_x_off;
                        else if (bbx_x_off < 0) val >>= (-bbx_x_off);
                        
                        final_char_bytes[dest_row] = (uint8_t)(val & 0xFF);
                    }
                }

                memcpy(&rom_data[offset], final_char_bytes, 8);
                char_count++;
            }
            current_encoding = -1;
            bbx_w = 8; bbx_h = 8; bbx_x_off = 0; bbx_y_off = 0;
        } else if (in_bitmap) {
            if (bitmap_row < 16) {
                unsigned int val;
                if (sscanf(line, "%x", &val) == 1) {
                    raw_bytes[bitmap_row] = (uint8_t)val;
                }
                bitmap_row++;
            }
        }
    }
    fclose(f);

    // 1. バイナリの書き出し
    FILE *out_b = fopen(out_bin, "wb");
    if (!out_b) {
        perror("Failed to open output binary file");
        free(rom_data);
        return 1;
    }
    fwrite(rom_data, 1, TOTAL_SIZE, out_b);
    fclose(out_b);
    printf("Binary saved to: %s (Processed %d chars)\n", out_bin, char_count);

    // 2. 目視確認用のPBM生成
    printf("Generating grid PBM for visual inspection...\n");
    FILE *out_p = fopen(out_pbm, "wb");
    if (out_p) {
        fprintf(out_p, "P4\n2048 2048\n");

        for (int y = 0; y < 2048; y++) {
            int char_row = y / 8;
            int local_y = y % 8;

            for (int char_col = 0; char_col < 256; char_col++) {
                int code = (char_row * 256) + char_col;
                int offset = code * 8;

                int is_empty = 1;
                for (int i = 0; i < 8; i++) {
                    if (rom_data[offset + i] != 0xFF) {
                        is_empty = 0;
                        break;
                    }
                }

                if (is_empty) {
                    fputc(0xFF, out_p);
                } else {
                    fputc(~rom_data[offset + local_y], out_p);
                }
            }
        }
        fclose(out_p);
        printf("PBM grid saved to: %s\n", out_pbm);
    }

    free(rom_data);
    printf("All tasks completed!\n");
    return 0;
}