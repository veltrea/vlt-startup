# vlt-tools (Startup Management) 仕様書

Windows の自動起動を管理するための 3 つの独立したツール群。

## 1. ツール構成

| ツール名 | 役割 | 登録先 |
| :--- | :--- | :--- |
| `vlt-startup.exe` | ショートカット管理 | `%APPDATA%\Microsoft\Windows\Start Menu\Programs\Startup` |
| `vlt-autorun.exe` | レジストリ管理 | `HKCU\Software\Microsoft\Windows\CurrentVersion\Run` |
| `vlt-schtask.exe` | タスクスケジューラ管理 | Windows Task Scheduler (`ONLOGON`) |

---

## 2. 共通インターフェース仕様

すべてのツールにおいて、サブコマンド（add/remove等）を廃止し、ハイフン形式のフラグを採用する。
一文字のショートフラグと、可読性を重視したロングフラグの両方をサポートする。

---

## 3. 各ツール詳細仕様

### 3.1 vlt-startup
- **用途**: スタートアップフォルダにショートカット (.lnk) を作成。
- **引数**:
  - `-a, --add <Path>`: ショートカットを追加。第一引数にパスを指定した場合も追加とみなす。
  - `-r, --remove <Name>`: ショートカットを削除。
  - `--name <str>`: ショートカット名（省略時はファイル名）。
  - `--args <str>`: プログラム引数。
  - `--admin`: 「管理者として実行」フラグをセット。

### 3.2 vlt-autorun
- **用途**: レジストリ Run キーによる起動管理。Windows 11 の有効/無効フラグをサポート。
- **引数**:
  - `-a, --add <Path>`: 項目を追加。
  - `-r, --remove <Name>`: 項目を削除（StartupApproved キーも含む）。
  - `-l, --list`: 現在の Run キー項目を表示。
  - `-e, --enable <Name>`: `StartupApproved` キーにより項目を有効化。
  - `-d, --disable <Name>`: `StartupApproved` キーにより項目を無効化。
  - `--name <str>`: レジストリ値の名前（省略時はファイル名）。
  - `--args <str>`: プログラム引数。

### 3.3 vlt-schtask
- **用途**: タスクスケジューラへの登録。管理者制限の回避に有効。
- **引数**:
  - `-a, --add <Path>`: ログオン時に実行されるタスクを作成。
  - `-r, --remove <Name>`: 指定したタスクを削除。
  - `--name <str>`: タスク名（省略時はファイル名）。
  - `--args <str>`: プログラム引数。
  - `--admin`: 「最上位の特権 (Highest Privileges)」で実行。

---

## 4. 技術要件
- 言語: C++ (MSVC)
- 依存: Win32 API, COM (IShellLinkオブジェクト), `schtasks.exe` (外部コマンド呼び出し)
- 対応OS: Windows 10/11 (Unicode)

---

## 5. ライセンス
- MIT License
