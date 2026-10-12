#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

const char path[] = "C:\\Users\\kojim\\Documents\\GitHub\\SONY_Walkman_NW-MS70D_Restomod\\walkman_logo_2b.PBM";
const char out_path[] = "C:\\Users\\kojim\\Documents\\GitHub\\SONY_Walkman_NW-MS70D_Restomod\\misaki_png_2021-05-05a\\walkman_logo_2b_output.bin";

int main(){
    // uint8_t *data_raw = fopen(path, "rb");
    FILE *data_pt = fopen(path, "rb");
    char buff[256];
    fgets(buff, sizeof(buff), data_pt);
    
    while (1) {
        // 現在の位置の1文字目をちょっと覗き見
        int c = fgetc(data_pt);
        if (c == '#') {
            ungetc(c, data_pt); // 戻して
            fgets(buff, sizeof(buff), data_pt); // 1行まるごと捨てる
        } else {
            ungetc(c, data_pt); // コメントじゃなければ戻してループ抜け
            break;
        }
    }

    // 3. サイズ（幅と高さ、例: "64 128"）の行を読み取る
    fgets(buff, sizeof(buff), data_pt); 
    int width, height;
    sscanf(buff, "%d %d", &width, &height);
    printf("%d %d\n", width, height);

    // 1行あたりのバイト数（8ピクセル未満の端数を切り上げる計算）
    int bytes_per_row = (width + 7) / 8;

    // 全体のデータサイズ
    int total_data_size = bytes_per_row * height;
    uint8_t *data_raw = malloc(total_data_size);
    uint8_t *data_output = malloc(total_data_size);
    fread(data_raw, 1, total_data_size, data_pt);
    fclose(data_pt);
    printf("%d %d\n", bytes_per_row, total_data_size);

    uint8_t *output_pt = data_output;

    // k: 8行ごとのブロック単位のループ
    for(int k = 0; k < (height / 8); k++){
        // j: 横方向のバイト単位のループ
        for(int j = 0; j < bytes_per_row; j++){
            
            // 各8行ブロックにおける、0〜7行目の「基準アドレス」を計算で安全に特定する
            uint8_t *r0 = data_raw + (k * 8 + 0) * bytes_per_row + j;
            uint8_t *r1 = data_raw + (k * 8 + 1) * bytes_per_row + j;
            uint8_t *r2 = data_raw + (k * 8 + 2) * bytes_per_row + j;
            uint8_t *r3 = data_raw + (k * 8 + 3) * bytes_per_row + j;
            uint8_t *r4 = data_raw + (k * 8 + 4) * bytes_per_row + j;
            uint8_t *r5 = data_raw + (k * 8 + 5) * bytes_per_row + j;
            uint8_t *r6 = data_raw + (k * 8 + 6) * bytes_per_row + j;
            uint8_t *r7 = data_raw + (k * 8 + 7) * bytes_per_row + j;

            // i: 縦方向（1バイト内）のビット並び替えループ
            for(int i = 0; i < 8; i++){
                *output_pt = 0;
                *output_pt |= (uint8_t)(((uint8_t)((*r0 >> (7 - i)) & 0x01)) << 0);
                *output_pt |= (uint8_t)(((uint8_t)((*r1 >> (7 - i)) & 0x01)) << 1);
                *output_pt |= (uint8_t)(((uint8_t)((*r2 >> (7 - i)) & 0x01)) << 2);
                *output_pt |= (uint8_t)(((uint8_t)((*r3 >> (7 - i)) & 0x01)) << 3);
                *output_pt |= (uint8_t)(((uint8_t)((*r4 >> (7 - i)) & 0x01)) << 4);
                *output_pt |= (uint8_t)(((uint8_t)((*r5 >> (7 - i)) & 0x01)) << 5);
                *output_pt |= (uint8_t)(((uint8_t)((*r6 >> (7 - i)) & 0x01)) << 6);
                *output_pt |= (uint8_t)(((uint8_t)((*r7 >> (7 - i)) & 0x01)) << 7);
                
                output_pt++;
            }
        }
    }
    FILE *out_pt = fopen(out_path, "wb");
    fwrite(data_output, 1, total_data_size, out_pt);
    fclose(out_pt);
    free(data_raw);
    free(data_output);

}