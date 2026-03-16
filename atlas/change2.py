import os

# ================= 配置区域 =================
ROOT_DIR = '.' 
# ===========================================

def force_rename_folders():
    if not os.path.exists(ROOT_DIR):
        print(f"错误：找不到目录 {ROOT_DIR}")
        return

    items = os.listdir(ROOT_DIR)
    folders = [f for f in items if os.path.isdir(os.path.join(ROOT_DIR, f))]
    
    if not folders:
        print("当前目录下没有找到任何子文件夹。")
        return

    print(f"发现 {len(folders)} 个文件夹，开始强制重命名（首字母转小写）...\n")

    count = 0
    for folder_name in folders:
        if folder_name.startswith('.'):
            continue
            
        # 构造新名字：首字母小写 + 剩余部分
        new_name = folder_name[0].lower() + folder_name[1:]
        
        # 如果名字本身没变，跳过
        if new_name == folder_name:
            continue

        old_path = os.path.join(ROOT_DIR, folder_name)
        new_path = os.path.join(ROOT_DIR, new_name)

        try:
            # 【关键】直接执行重命名，不做任何存在性检查
            # 在 Windows/macOS 上，这会将 "Sacrifice" 刷新为 "sacrifice"
            os.rename(old_path, new_path)
            print(f"[OK] {folder_name} -> {new_name}")
            count += 1
        except Exception as e:
            # 只有在真的出错（如权限不足）时才打印
            print(f"[ERROR] {folder_name}: {e}")

    print("\n" + "="*30)
    print(f"强制处理完成！共修正 {count} 个文件夹名称。")
    print("提示：如果资源管理器中显示未更新，请刷新一下（F5）。")

if __name__ == '__main__':
    force_rename_folders()