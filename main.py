from fastapi.middleware.cors import CORSMiddleware
from fastapi import FastAPI
from fastapi.staticfiles import StaticFiles
from dotenv import load_dotenv
import os

app = FastAPI()

app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_methods=["*"],
    allow_headers=["*"],
)

@app.get("/config")
def get_config():
    return {
        "EXE_URL": os.getenv("EXE_URL"),
        "API_URL": os.getenv("API_URL")
    }

# 静的ファイルを "/static" にマウント（index.html などを含む）
app.mount("/", StaticFiles(directory="public", html=True), name="static")