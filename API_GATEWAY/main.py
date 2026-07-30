import os
import secrets
import psycopg2
import redis
from fastapi import FastAPI, HTTPException
from pydantic import BaseModel

app = FastAPI(title="KungFu Chess API Gateway")

# הגדרות התחברות
DB_HOST = os.getenv("DB_HOST", "db")
DB_PORT = os.getenv("DB_PORT", "5432")
DB_NAME = os.getenv("DB_NAME", "kungfu_chess")
DB_USER = os.getenv("DB_USER", "admin")
DB_PASSWORD = os.getenv("DB_PASSWORD", "secretpassword")

REDIS_HOST = os.getenv("REDIS_HOST", "redis")
REDIS_PORT = int(os.getenv("REDIS_PORT", "6379"))

# יצירת חיבור ל-Redis
redis_client = redis.Redis(host=REDIS_HOST, port=REDIS_PORT, decode_responses=True)

def get_db_connection():
    return psycopg2.connect(
        host=DB_HOST, database=DB_NAME, user=DB_USER, password=DB_PASSWORD, port=DB_PORT
    )

@app.on_event("startup")
def startup_event():
    try:
        conn = get_db_connection()
        cur = conn.cursor()
        cur.execute("""
            CREATE TABLE IF NOT EXISTS users (
                id SERIAL PRIMARY KEY,
                username VARCHAR(50) UNIQUE NOT NULL,
                password VARCHAR(255) NOT NULL,
                rating INTEGER DEFAULT 1200
            );
        """)
        conn.commit()
        cur.close()
        conn.close()
        print("[API GATEWAY] Connected to PostgreSQL and verified 'users' table.")
    except Exception as e:
        print(f"[API GATEWAY ERROR] Failed to connect to database: {e}")

class UserRequest(BaseModel):
    username: str
    password: str

@app.get("/health")
async def health_check():
    return {"status": "ok", "service": "API Gateway is running"}

@app.post("/api/register")
async def register(user: UserRequest):
    try:
        conn = get_db_connection()
        cur = conn.cursor()
        cur.execute("SELECT id FROM users WHERE username = %s;", (user.username,))
        if cur.fetchone():
            raise HTTPException(status_code=400, detail="Username already exists")

        cur.execute(
            "INSERT INTO users (username, password, rating) VALUES (%s, %s, 1200) RETURNING id, rating;",
            (user.username, user.password)
        )
        new_user = cur.fetchone()
        conn.commit()
        cur.close()
        conn.close()

        return {
            "status": "success",
            "message": "User registered successfully",
            "userId": new_user[0],
            "username": user.username,
            "rating": new_user[1]
        }
    except Exception as e:
        if isinstance(e, HTTPException):
            raise e
        raise HTTPException(status_code=500, detail=str(e))

@app.post("/api/login")
async def login(user: UserRequest):
    try:
        conn = get_db_connection()
        cur = conn.cursor()
        cur.execute("SELECT id, password, rating FROM users WHERE username = %s;", (user.username,))
        db_user = cur.fetchone()
        cur.close()
        conn.close()

        if not db_user or db_user[1] != user.password:
            raise HTTPException(status_code=401, detail="Invalid username or password")

        # יצירת Token אקראי ושמירתו ב-Redis ל-24 שעות
        token = secrets.token_hex(16)
        redis_key = f"session:{token}"
        redis_client.setex(redis_key, 86400, user.username)

        return {
            "status": "success",
            "message": "Login successful",
            "userId": db_user[0],
            "username": user.username,
            "rating": db_user[2],
            "token": token
        }
    except Exception as e:
        if isinstance(e, HTTPException):
            raise e
        raise HTTPException(status_code=500, detail=str(e))