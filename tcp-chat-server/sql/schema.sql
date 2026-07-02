CREATE TABLE IF NOT EXISTS user (
    id TEXT PRIMARY KEY,
    username VARCHAR(32) NOT NULL,
    password TEXT,
    created_at  TIMESTAMP DEFAULT CURRENT_TIMESTAMP NOT NULL
);

CREATE TABLE IF NOT EXISTS room (
    id TEXT PRIMARY KEY,
    roomname VARCHAR(32) NOT NULL,
    created_at  TIMESTAMP DEFAULT CURRENT_TIMESTAMP NOT NULL
    --capacity INTEGER,
);

CREATE TABLE IF NOT EXISTS message (
    id TEXT PRIMARY KEY,
    sender_id TEXT NOT NULL,
    room_id TEXT NOT NULL,
    content TEXT NOT NULL,
    sent_at INTEGER NOT NULL,

    FOREIGN KEY (room_id) REFERENCES room(id),
    FOREIGN KEY (sender_id) REFERENCES user(id)
);

CREATE TABLE IF NOT EXISTS room_member (
    room_id TEXT NOT NULL,
    user_id TEXT NOT NULL,

    PRIMARY KEY (room_id, user_id),

    FOREIGN KEY (room_id) REFERENCES room(id),
    FOREIGN KEY (user_id) REFERENCES user(id)
)


--CREATE TABLE IF NOT EXISTS friendship