# Pythonの軽量イメージをベースに
FROM python:3.11-slim

# 環境変数設定（UTF-8など）
ENV PYTHONDONTWRITEBYTECODE=1 \
    PYTHONUNBUFFERED=1

# 作業ディレクトリを作成
WORKDIR /app

# 依存ファイルのコピーとインストール
COPY requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt

# ソースコードと静的ファイルをコピー
COPY . .

# ポート開放（Render.comでは自動認識されるが明示すると安全）
EXPOSE 8000

# アプリ起動（main.py内の `app` を参照）
CMD ["uvicorn", "main:app", "--host", "0.0.0.0", "--port", "8000"]