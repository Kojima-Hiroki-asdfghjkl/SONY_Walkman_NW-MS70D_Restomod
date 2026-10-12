def generate_flash_adder_buff(text, array_size=128):
    lines = []
    # 配列サイズを超えないように切り詰める
    target_text = text[:array_size]
    actual_len = len(target_text) # 実際に初期化する要素数（文字数）をカウント
    
    for ch in target_text:
        code = ord(ch)
        addr = code << 3
        lines.append(f"    0x{addr:06X}, // '{ch}' (0x{code:04X} << 3)")
        
    # 配列の総バイト数（128要素 × 3バイト = 384バイト）
    total_bytes = array_size * 3
    
    output = f"uint24_t flashAdderBuff[{array_size}] = {{\n"
    output += "\n".join(lines)
    output += f"\n}};//{total_bytes}\n"
    
    # 最後に要素数を標準出力やデバッグ用に付加（あるいは別途print）
    print(f"--- 要素数 (Active Elements): {actual_len} ---")
    
    return output

if __name__ == "__main__":
    target_text = "或暮秋の日の事である。一人の下人が、羅生門の下で雨やみを待っていた。"
    
    result = generate_flash_adder_buff(target_text)
    print(result)