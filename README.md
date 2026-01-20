# vlt-tools - Windows Startup Management Suite v0.1.0

Windows 11 でアクセスが面倒になったスタートアップフォルダや、複数の登録方法（フォルダ、レジストリ、タスクスケジューラ）を、コマンドラインから一括して効率的に管理するためのツールセットです。
直感的なフラグ操作により、環境構築やスクリプトからの自動設定を容易にします。

- **Repository**: [https://github.com/veltrea/vlt-startup](https://github.com/veltrea/vlt-startup)
- **Author**: veltrea <veltrea@outlook.com>

## 収録ツール

### 1. vlt-startup.exe
スタートアップフォルダにショートカットを作成します。最も視認性が高い方法です。

```batch
:: 追加 (ショート/ロング/フラグなし)
vlt-startup.exe -a "C:\App\MyApp.exe" --name "MyLauncher" --admin
vlt-startup.exe --add "C:\App\MyApp.exe"
vlt-startup.exe "C:\App\MyApp.exe"

:: 削除
vlt-startup.exe -r "MyLauncher"
vlt-startup.exe --remove "MyLauncher"
```

### 2. vlt-autorun.exe
レジストリ（Runキー）への登録を行います。Windows 11 のスタートアップ画面でのオン/オフ制御に対応しています。

```batch
:: 追加
vlt-autorun.exe -a "C:\App\MyApp.exe" --name "MyService"

:: 一覧表示
vlt-autorun.exe -l
vlt-autorun.exe --list

:: 無効化 / 有効化 (Windows 11 設定アプリ互換)
vlt-autorun.exe --disable "MyService"
vlt-autorun.exe --enable "MyService"

:: 削除
vlt-autorun.exe --remove "MyService"
```

### 3. vlt-schtask.exe
タスクスケジューラにログオン時実行タスクを作成します。管理者権限での実行（UAC回避）に最適です。

```batch
:: 追加 (管理者特権あり)
vlt-schtask.exe --add "C:\App\AdminTool.exe" --admin

:: 削除
vlt-schtask.exe --remove "AdminTool"
```

---

## 共通オプション

- `-a, --add <path>`: 追加
- `-r, --remove <name>`: 削除
- `--name <string>`: 登録名を指定（デフォルトはファイル名）
- `--args <string>`: 起動時の引数を指定

---

## ビルド方法

`build.bat` を実行してください（MSVC環境が必要）。
バイナリは `bin/` フォルダに出力されます。

---

## ライセンス

[MIT License](LICENSE)
