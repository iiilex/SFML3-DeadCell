import os
from PIL import Image
import sys

# ================= 配置区域 (在此处修改) =================
# 要处理的文件夹路径
# 可以是相对路径 (如 "sprites") 或绝对路径 (如 "C:/Game/Assets")
TARGET_FOLDER = "player"

# 是否处理子目录？(True = 递归所有子文件夹, False = 只处理当前层)
RECURSIVE = True
# =======================================================

def process_single_image(file_path):
    """处理单张图片并覆盖"""
    try:
        # 打开并转换为 RGBA (确保有 Alpha 通道)
        img = Image.open(file_path).convert("RGBA")
        width, height = img.size
        
        # 获取非透明区域边界
        bbox = img.split()[3].getbbox()

        if bbox is None:
            # 完全透明，跳过
            return False, "跳过 (全透明)"

        left, upper, right, lower = bbox
        new_width = right - left
        new_height = lower - upper

        # 如果裁剪后大小没变（即原本就没有透明边），也跳过保存操作以节省 IO
        if new_width == width and new_height == height:
            return False, "跳过 (无边距)"

        # 执行裁剪
        cropped_img = img.crop(bbox)
        
        # 【关键】直接覆盖原文件
        # PNG 保存时优化压缩，通常文件体积也会变小
        cropped_img.save(file_path, optimize=True)
        
        return True, f"已裁剪: {width}x{height} -> {new_width}x{new_height}"

    except Exception as e:
        return False, f"❌ 错误: {str(e)}"

def main():
    # 1. 检查文件夹是否存在
    if not os.path.exists(TARGET_FOLDER):
        print(f"❌ 错误：找不到文件夹 '{TARGET_FOLDER}'")
        print(f"   当前程序运行目录: {os.getcwd()}")
        print("   请修改代码第 14 行的 TARGET_FOLDER 路径。")
        input("\n按回车键退出...")
        return

    print(f"🚀 开始扫描文件夹: {os.path.abspath(TARGET_FOLDER)}")
    print("⚠️  警告：所有文件将被直接覆盖！请确保已备份！")
    print("-" * 50)

    count_total = 0
    count_processed = 0
    count_skipped = 0
    count_errors = 0

    # 2. 遍历文件
    # walk_topdown=True 表示从上往下遍历
    for root, dirs, files in os.walk(TARGET_FOLDER):
        for filename in files:
            if filename.lower().endswith('.png'):
                full_path = os.path.join(root, filename)
                count_total += 1
                
                # 显示进度 (每处理 10 个或最后一个显示一次，避免刷屏太快)
                status_msg = f"[{count_total}] 处理: {full_path} ..."
                print(status_msg, end="\r") 
                
                success, msg = process_single_image(full_path)
                
                if success:
                    count_processed += 1
                    print(f"✅ [{count_total}] {full_path} -> {msg}   ")
                else:
                    count_skipped += 1
                    # 只有当不是"跳过"而是"错误"时，或者你想看所有跳过信息时才打印
                    if "错误" in msg:
                        count_errors += 1
                        print(f"❌ [{count_total}] {full_path} -> {msg}   ")
                    # 如果想看跳过的详细信息，取消下面这行的注释
                    # else:
                    #     print(f"⚠️  [{count_total}] {full_path} -> {msg}   ")

    # 3. 总结报告
    print("-" * 50)
    print("🎉 处理完成!")
    print(f"   扫描总数: {count_total}")
    print(f"   成功裁剪: {count_processed}")
    print(f"   自动跳过: {count_skipped} (无透明边或全透明)")
    print(f"   发生错误: {count_errors}")
    print("-" * 50)
    
    if count_errors > 0:
        print("⚠️  上方列出了错误详情，请检查。")
    
    input("\n按回车键退出...")

if __name__ == "__main__":
    # 简单的防误触确认（可选，如果不想每次确认可注释掉下面 3 行）
    # confirm = input(f"确定要覆盖处理 '{TARGET_FOLDER}' 中的所有图片吗？(输入 yes 继续): ")
    # if confirm != "yes":
    #     print("已取消。")
    #     sys.exit(0)
    
    main()