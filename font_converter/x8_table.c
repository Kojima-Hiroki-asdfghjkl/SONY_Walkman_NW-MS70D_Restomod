#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

const char path[] = "C:\\Users\\kojim\\Documents\\GitHub\\SONY_Walkman_NW-MS70D_Restomod\\misaki_png_2021-05-05a\\walkman_logo_2b_output.bin";
const char out_path[] = "C:\\Users\\kojim\\Documents\\GitHub\\SONY_Walkman_NW-MS70D_Restomod\\misaki_png_2021-05-05a\\walkman_logo_2b_output.txt";
char valName[] = "misaki_8x8_font";

uint32_t get_file_size(FILE *fp);

int main(){
    FILE *data_pt = fopen(path, "rb");
    if (data_pt == NULL) {
        perror("Failed to open input file");
        return 1;
    }

    // 出力用テキストファイルを開く
    FILE *out_pt = fopen(out_path, "w");
    if (out_pt == NULL) {
        perror("Failed to open output file");
        fclose(data_pt);
        return 1;
    }

    uint32_t file_size = get_file_size(data_pt);

    // 画面ではなくテキストファイルに書き込む (fprintfを使用)
    fprintf(out_pt, "const uint8_t %s[] = {", valName);
    
    uint32_t count = 0;
    for (uint32_t i = 0; i < file_size; i++) {
        if (count % 16 == 0) {
            fprintf(out_pt, "\n\t");
        }
        
        int byte = fgetc(data_pt);
        if (byte == EOF) break;
        
        fprintf(out_pt, "0x%02X, ", (uint8_t)byte);
        count++;
    }
    
    if (count > 0) {
        fprintf(out_pt, "\b\b \n"); // 最後のカンマを消すためのバックスペース
    }
    fprintf(out_pt, "};\n");

    // ファイルを閉じる
    fclose(data_pt);
    fclose(out_pt);
    return 0;
}

uint32_t get_file_size(FILE *fp) {
    long current = ftell(fp);       // 現在地を保存
    fseek(fp, 0, SEEK_END);         // 末尾に移動
    long size = ftell(fp);          // 末尾の位置（＝サイズ）を取得
    fseek(fp, current, SEEK_SET);   // 元の位置に戻す
    return (uint32_t)size;
}