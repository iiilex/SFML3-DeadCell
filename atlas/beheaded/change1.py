import os
import shutil

# ================= 配置区域 =================
# 图片所在的目录，'.' 代表当前目录
SOURCE_DIR = '.' 
# 目标根目录，如果希望直接在当前目录下生成子文件夹，保持为 None 或 '.'
TARGET_ROOT = SOURCE_DIR 

# 是否自动补零帧号？(例如: 1.png -> 001.png, 10.png -> 010.png)
# 设为 True 可以确保在播放器或引擎中按正确顺序排列
PAD_FRAMES = True 
FRAME_WIDTH = 3  # 补零后的位数，例如 3 代表 000-999
# ===========================================

def organize_sprites():
    if not os.path.exists(SOURCE_DIR):
        print(f"错误：找不到目录 {SOURCE_DIR}")
        return

    # 获取所有 png 文件 (不区分大小写)
    files = [f for f in os.listdir(SOURCE_DIR) if f.lower().endswith('.png')]
    
    if not files:
        print("当前目录下没有找到 .png 文件。")
        return

    print(f"发现 {len(files)} 个图片文件，开始整理...\n")

    success_count = 0
    skip_count = 0
    
    for filename in files:
        # 跳过脚本文件本身
        if filename.endswith('.py'):
            continue

        # 核心逻辑：分割文件名
        # 假设格式严格为：Name_Frame.png
        name_part, ext = os.path.splitext(filename)
        
        if '_' not in name_part:
            print(f"[跳过] 文件名中缺少下划线 '_': {filename}")
            skip_count += 1
            continue

        # 从右边分割一次，防止动作名里也有下划线 (例如: char_jump_01.png -> 动作名: char_jump, 帧: 01)
        # rsplit('_', 1) 表示从右侧开始分割，只分割一次
        parts = name_part.rsplit('_', 1)
        
        if len(parts) != 2:
            print(f"[跳过] 文件名格式异常: {filename}")
            skip_count += 1
            continue

        action_name = parts[0]      # 动作名 (例如: activationLevier)
        frame_str = parts[1]        # 帧号字符串 (例如: 00 或 5)

        # 验证帧号是否为数字，防止误判
        if not frame_str.isdigit():
            print(f"[跳过] 帧号部分不是数字: {filename} (识别到的帧号: '{frame_str}')")
            skip_count += 1
            continue

        # 处理帧号补零 (可选)
        if PAD_FRAMES:
            try:
                frame_num = int(frame_str)
                new_frame_str = f"{frame_num:0{FRAME_WIDTH}d}"
            except ValueError:
                new_frame_str = frame_str # 如果转换失败，保持原样
        else:
            new_frame_str = frame_str

        # 构建目标路径
        target_folder = os.path.join(TARGET_ROOT, action_name)
        new_filename = f"{new_frame_str}.png"
        target_path = os.path.join(target_folder, new_filename)

        # 如果文件已经在那了且名字一样，跳过
        src_path = os.path.join(SOURCE_DIR, filename)
        if os.path.abspath(src_path) == os.path.abspath(target_path):
            continue

        # 创建文件夹
        if not os.path.exists(target_folder):
            os.makedirs(target_folder)
            print(f"创建文件夹: {action_name}/")

        # 移动文件
        try:
            shutil.move(src_path, target_path)
            print(f"[OK] {filename} -> {action_name}/{new_filename}")
            success_count += 1
        except Exception as e:
            print(f"[ERROR] 移动失败 {filename}: {e}")

    print("\n" + "="*30)
    print(f"整理完成！")
    print(f"成功移动: {success_count} 个文件")
    print(f"跳过/错误: {skip_count} 个文件")
    print(f"结果目录结构示例:")
    if success_count > 0:
        # 简单展示第一个创建的文件夹内容
        first_folder = os.path.join(TARGET_ROOT, os.listdir(TARGET_ROOT)[0] if os.listdir(TARGET_ROOT) else "")
        if os.path.isdir(first_folder):
            print(f"  {os.path.basename(first_folder)}/")
            sample_files = sorted(os.listdir(first_folder))[:5]
            for f in sample_files:
                print(f"    - {f}")
            if len(os.listdir(first_folder)) > 5:
                print("    ...")

if __name__ == '__main__':
    organize_sprites()