#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include "pustaka.h"

// Fungsi untuk membuat tabel
int buat_tabel(sqlite3 *db) {
    char *error_msg = 0;
    const char *sql = 
        "CREATE TABLE IF NOT EXISTS buku ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "judul TEXT NOT NULL, "
        "penulis TEXT NOT NULL, "
        "tahun INTEGER, "
        "kategori TEXT, "
        "status_tersedia INTEGER DEFAULT 1);";
    
    int rc = sqlite3_exec(db, sql, 0, 0, &error_msg);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", error_msg);
        sqlite3_free(error_msg);
        return 1;
    }
    return 0;
}

// Fungsi untuk menambah buku
int tambah_buku(sqlite3 *db, Buku *buku) {
    // Validasi input
    if (!validasi_input(buku)) {
        printf("Data buku tidak valid!\n");
        return 1;
    }

    sqlite3_stmt *stmt;
    const char *sql = "INSERT INTO buku (judul, penulis, tahun, kategori, status_tersedia) VALUES (?, ?, ?, ?, 1);";
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Gagal menyiapkan statement: %s\n", sqlite3_errmsg(db));
        return 1;
    }
    
    sqlite3_bind_text(stmt, 1, buku->judul, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, buku->penulis, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, buku->tahun);
    sqlite3_bind_text(stmt, 4, buku->kategori, -1, SQLITE_STATIC);
    
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Gagal menambah buku: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return 1;
    }
    
    sqlite3_finalize(stmt);
    printf("Buku berhasil ditambahkan!\n");
    return 0;
}

// Callback untuk menampilkan buku
int callback_tampil_buku(void *data, int argc, char **argv, char **azColName) {
    printf("ID: %s | Judul: %s | Penulis: %s | Tahun: %s | Kategori: %s | Status: %s\n", 
           argv[0], argv[1], argv[2], argv[3], argv[4], 
           (strcmp(argv[5], "1") == 0 ? "Tersedia" : "Dipinjam"));
    return 0;
}

// Fungsi untuk menampilkan semua buku
int tampilkan_buku(sqlite3 *db) {
    const char *sql = "SELECT * FROM buku;";
    char *error_msg = 0;
    
    printf("\n--- DAFTAR BUKU ---\n");
    int rc = sqlite3_exec(db, sql, callback_tampil_buku, 0, &error_msg);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", error_msg);
        sqlite3_free(error_msg);
        return 1;
    }
    return 0;
}

// Fungsi untuk mencari buku berdasarkan judul
int cari_buku(sqlite3 *db, const char *judul) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT * FROM buku WHERE judul LIKE ?;";
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Gagal menyiapkan statement: %s\n", sqlite3_errmsg(db));
        return 1;
    }
    
    char search_pattern[MAX_STR];
    snprintf(search_pattern, sizeof(search_pattern), "%%%s%%", judul);
    sqlite3_bind_text(stmt, 1, search_pattern, -1, SQLITE_STATIC);
    
    printf("\n--- HASIL PENCARIAN ---\n");
    int found = 0;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        printf("ID: %d | Judul: %s | Penulis: %s | Tahun: %d | Kategori: %s | Status: %s\n", 
               sqlite3_column_int(stmt, 0),
               sqlite3_column_text(stmt, 1),
               sqlite3_column_text(stmt, 2),
               sqlite3_column_int(stmt, 3),
               sqlite3_column_text(stmt, 4),
               (sqlite3_column_int(stmt, 5) == 1 ? "Tersedia" : "Dipinjam"));
        found = 1;
    }
    
    sqlite3_finalize(stmt);
    
    if (!found) {
        printf("Tidak ada buku ditemukan.\n");
    }
    return 0;
}

// Fungsi untuk meminjam buku
int pinjam_buku(sqlite3 *db, int id_buku) {
    sqlite3_stmt *stmt;
    const char *sql_pinjam = "UPDATE buku SET status_tersedia = 0 WHERE id = ? AND status_tersedia = 1;";
    
    int rc = sqlite3_prepare_v2(db, sql_pinjam, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Gagal menyiapkan statement: %s\n", sqlite3_errmsg(db));
        return 1;
    }
    
    sqlite3_bind_int(stmt, 1, id_buku);
    rc = sqlite3_step(stmt);
    
    if (rc == SQLITE_DONE && sqlite3_changes(db) > 0) {
        printf("Buku berhasil dipinjam!\n");
    } else {
        printf("Gagal meminjam buku. Mungkin buku sudah dipinjam atau tidak ditemukan.\n");
    }
    
    sqlite3_finalize(stmt);
    return 0;
}

// Fungsi untuk mengembalikan buku
int kembalikan_buku(sqlite3 *db, int id_buku) {
    sqlite3_stmt *stmt;
    const char *sql_kembali = "UPDATE buku SET status_tersedia = 1 WHERE id = ? AND status_tersedia = 0;";
    
    int rc = sqlite3_prepare_v2(db, sql_kembali, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Gagal menyiapkan statement: %s\n", sqlite3_errmsg(db));
        return 1;
    }
    
    sqlite3_bind_int(stmt, 1, id_buku);
    rc = sqlite3_step(stmt);
    
    if (rc == SQLITE_DONE && sqlite3_changes(db) > 0) {
        printf("Buku berhasil dikembalikan!\n");
    } else {
        printf("Gagal mengembalikan buku. Mungkin buku sudah tersedia atau tidak ditemukan.\n");
    }
    
    sqlite3_finalize(stmt);
    return 0;
}

// Fungsi untuk menampilkan menu
void tampilkan_menu() {
    printf("\n--- SISTEM PERPUSTAKAAN DIGITAL ---\n");
    printf("1. Tambah Buku\n");
    printf("2. Tampilkan Buku\n");
    printf("3. Cari Buku\n");
    printf("4. Pinjam Buku\n");
    printf("5. Kembalikan Buku\n");
    printf("6. Keluar\n");
    printf("Pilih menu (1-6): ");
}

// Fungsi untuk input buku
Buku input_buku() {
    Buku buku;
    
    printf("Judul Buku: ");
    scanf(" %[^\n]s", buku.judul);
    
    printf("Penulis Buku: ");
    scanf(" %[^\n]s", buku.penulis);
    
    printf("Tahun Terbit: ");
    scanf("%d", &buku.tahun);
    
    printf("Kategori Buku: ");
    scanf(" %[^\n]s", buku.kategori);
    
    return buku;
}

// Fungsi validasi input buku
int validasi_input(Buku *buku) {
    // Array kriteria validasi
    char *kriteria[] = {
        "Judul harus diisi",
        "Penulis harus diisi", 
        "Tahun terbit harus valid"
    };
    
    // Pengulangan untuk validasi
    int valid = 1;
    if (strlen(buku->judul) == 0) {
        printf("%s\n", kriteria[0]);
        valid = 0;
    }
    
    if (strlen(buku->penulis) == 0) {
        printf("%s\n", kriteria[1]);
        valid = 0;
    }
    
    if (buku->tahun < 1800 || buku->tahun > 2024) {
        printf("%s\n", kriteria[2]);
        valid = 0;
    }
    
    return valid;
}