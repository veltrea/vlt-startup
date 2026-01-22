# vlt-tools - Windows Startup Management Suite v0.1.0

Windows 11 でアクセスが面倒になったスタートアップ管理を、コマンドラインから効率化するためのツール群です。

「ログイン時に特定のプログラムを起動する」というシンプルな目的を確実に達成するため、Windows の将来的な仕様変更（スタートアップフォルダやレジストリの廃止など）を見据え、現在利用可能な 3 つの主要な手法（フォルダ、レジストリ、タスクスケジューラ）を選択できるようにしています。

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

## オプションの詳細と活用例

### 1. プログラム引数の指定 (`--args`)

起動するプログラムに対してオプションを渡すことができます。

- **ブラウザで特定のサイトを開く**:
  `vlt-startup --add "C:\...\chrome.exe" --args "https://github.com"`
  
- **アプリを最小化状態で起動する** (アプリ側が対応している場合):
  `vlt-autorun --add "C:\...\app.exe" --args "--minimized"`

### 2. 管理者権限での実行 (`--admin`)

- **vlt-startup**: 作成されたショートカットに「管理者として実行」フラグを立てます。ログイン時に UAC 昇格確認が表示されるようになります。

- **vlt-schtask**: タスクを「最上位の特権」で実行するように設定します。これは UAC のポップアップを回避して管理者権限でアプリを自動起動させたい場合に非常に有効です。

---

## ビルド方法

`build.bat` を実行してください（MSVC環境が必要）。
バイナリは `bin/` フォルダに出力されます。

---

## ライセンス

[MIT License](LICENSE)
