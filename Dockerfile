FROM python:3.11-slim

# 作業ディレクトリ
WORKDIR /app

# 依存ファイルをコピー
COPY requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt

# アプリケーションと.envをコピー
COPY . .

# .envを環境変数として読み込む（アプリ内でload_dotenv()を使用するのでOK）

# 起動コマンド（ポート8000）
CMD ["uvicorn", "app.main:app", "--host", "0.0.0.0", "--port", "8000"]