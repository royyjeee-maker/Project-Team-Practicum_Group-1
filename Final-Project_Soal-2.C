#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_BUKU 1000
#define MAX_HISTORY 5000

typedef struct {
    char kode[20];
    char nama[100];
    char jenis[50];
    double harga;
} Buku;

typedef struct {
    char kode[20];
    int jumlah;
    double total;
} History;

Buku buku[MAX_BUKU];
History history[MAX_HISTORY];
int jumlahBuku = 0;
int jumlahHistory = 0;

/* Membaca satu baris teks, termasuk spasi, lalu membuang Enter. */
int bacaTeks(char teks[], int ukuran) {
    int c;

    if (fgets(teks, ukuran, stdin) == NULL) {
        return 0;
    }

    if (strchr(teks, '\n') == NULL) {
        while ((c = getchar()) != '\n' && c != EOF) {
            /* Membuang sisa input yang terlalu panjang. */
        }
    }

    teks[strcspn(teks, "\r\n")] = '\0';
    return 1;
}

/* Membaca bilangan bulat positif. Mengembalikan -1 jika input berakhir. */
int bacaAngka(void) {
    char teks[100];
    char sisa;
    int angka;

    while (bacaTeks(teks, sizeof(teks))) {
        if (sscanf(teks, "%d %c", &angka, &sisa) == 1 &&
            angka >= 1 && angka <= 1000000) {
            return angka;
        }

        printf("Masukkan bilangan bulat positif: ");
    }

    return -1;
}

/* Membaca data buku dan history. File yang belum ada dianggap kosong.
   Header boleh ada atau tidak ada pada file masukan. */
void bacaFile(void) {
    FILE *file;
    char baris[300];
    Buku dataBuku;
    History dataHistory;

    file = fopen("databuku.txt", "r");
    if (file != NULL) {
        while (jumlahBuku < MAX_BUKU &&
               fgets(baris, sizeof(baris), file) != NULL) {

            if (strncmp(baris, "KodeBuku|", 9) == 0) {
                continue;
            }

            if (sscanf(baris, "%19[^|]|%99[^|]|%49[^|]|%lf",
                       dataBuku.kode, dataBuku.nama, dataBuku.jenis,
                       &dataBuku.harga) == 4 &&
                dataBuku.harga > 0) {
                buku[jumlahBuku] = dataBuku;
                jumlahBuku++;
            } else {
                printf("Baris data buku tidak valid dan dilewati.\n");
            }
        }
        fclose(file);
    }

    file = fopen("history.txt", "r");
    if (file != NULL) {
        while (jumlahHistory < MAX_HISTORY &&
               fgets(baris, sizeof(baris), file) != NULL) {

            if (strncmp(baris, "KodeBuku|", 9) == 0) {
                continue;
            }

            if (sscanf(baris, "%19[^|]|%d|%lf",
                       dataHistory.kode, &dataHistory.jumlah,
                       &dataHistory.total) == 3 &&
                dataHistory.jumlah > 0 && dataHistory.total > 0) {
                history[jumlahHistory] = dataHistory;
                jumlahHistory++;
            } else {
                printf("Baris data history tidak valid dan dilewati.\n");
            }
        }
        fclose(file);
    }
}

/* Menyimpan seluruh data buku beserta header ke databuku.txt. */
int simpanBuku(void) {
    FILE *file = fopen("databuku.txt", "w");
    int i;

    if (file == NULL) {
        return 0;
    }

    fprintf(file, "KodeBuku|NamaBuku|JenisBuku|Harga\n");

    for (i = 0; i < jumlahBuku; i++) {
        if (fprintf(file, "%s|%s|%s|%.2f\n",
                    buku[i].kode, buku[i].nama,
                    buku[i].jenis, buku[i].harga) < 0) {
            fclose(file);
            return 0;
        }
    }

    return fclose(file) == 0;
}

/* Menyimpan seluruh transaksi beserta header ke history.txt. */
int simpanHistory(void) {
    FILE *file = fopen("history.txt", "w");
    int i;

    if (file == NULL) {
        return 0;
    }

    fprintf(file, "KodeBuku|JumlahTerjual|TotalHarga\n");

    for (i = 0; i < jumlahHistory; i++) {
        if (fprintf(file, "%s|%d|%.2f\n",
                    history[i].kode, history[i].jumlah,
                    history[i].total) < 0) {
            fclose(file);
            return 0;
        }
    }

    return fclose(file) == 0;
}

/* Mengambil nomor terbesar dari kode BK, agar kode baru tidak duplikat
   dan nomor tetap berlanjut meskipun data buku pernah dihapus. */
void buatKode(char kode[]) {
    int nomorTerbesar = 0;
    int nomor;
    int i;

    for (i = 0; i < jumlahBuku; i++) {
        if (strncmp(buku[i].kode, "BK", 2) == 0) {
            nomor = atoi(buku[i].kode + 2);
            if (nomor > nomorTerbesar) {
                nomorTerbesar = nomor;
            }
        }
    }

    for (i = 0; i < jumlahHistory; i++) {
        if (strncmp(history[i].kode, "BK", 2) == 0) {
            nomor = atoi(history[i].kode + 2);
            if (nomor > nomorTerbesar) {
                nomorTerbesar = nomor;
            }
        }
    }

    sprintf(kode, "BK%03d", nomorTerbesar + 1);
}

/* Menu 1: input buku baru. */
void inputBuku(void) {
    Buku baru;
    char teks[100];
    char sisa;

    if (jumlahBuku >= MAX_BUKU) {
        printf("Penyimpanan buku penuh.\n");
        return;
    }

    do {
        printf("Nama buku: ");
        if (!bacaTeks(baru.nama, sizeof(baru.nama))) {
            return;
        }
        if (strlen(baru.nama) == 0 || strchr(baru.nama, '|') != NULL) {
            printf("Nama tidak boleh kosong atau mengandung karakter |.\n");
        }
    } while (strlen(baru.nama) == 0 || strchr(baru.nama, '|') != NULL);

    do {
        printf("Jenis buku: ");
        if (!bacaTeks(baru.jenis, sizeof(baru.jenis))) {
            return;
        }
        if (strlen(baru.jenis) == 0 || strchr(baru.jenis, '|') != NULL) {
            printf("Jenis tidak boleh kosong atau mengandung karakter |.\n");
        }
    } while (strlen(baru.jenis) == 0 || strchr(baru.jenis, '|') != NULL);

    while (1) {
        printf("Harga buku: ");
        if (!bacaTeks(teks, sizeof(teks))) {
            return;
        }

        if (sscanf(teks, "%lf %c", &baru.harga, &sisa) == 1 &&
            baru.harga > 0 && baru.harga <= 1000000000.0) {
            break;
        }

        printf("Harga harus berupa angka lebih dari 0.\n");
    }

    buatKode(baru.kode);
    buku[jumlahBuku] = baru;
    jumlahBuku++;

    printf("Buku berhasil ditambahkan. Kode buku: %s\n", baru.kode);
}

/* Menu 3: menampilkan seluruh data buku. */
void viewBuku(void) {
    int i;

    if (jumlahBuku == 0) {
        printf("Belum ada data buku.\n");
        return;
    }

    printf("\n========== DAFTAR BUKU ==========\n");
    printf("No. | Kode Buku | Nama Buku | Jenis Buku | Harga\n");

    for (i = 0; i < jumlahBuku; i++) {
        printf("%d. | %s | %s | %s | Rp %.2f\n",
               i + 1, buku[i].kode, buku[i].nama,
               buku[i].jenis, buku[i].harga);
    }
}

/* Menu 2: menampilkan seluruh histori transaksi. */
void viewHistory(void) {
    int i;

    if (jumlahHistory == 0) {
        printf("Belum ada history transaksi.\n");
        return;
    }

    printf("\n========== HISTORY TRANSAKSI ==========\n");
    printf("No. | Kode Buku | Jumlah Terjual | Total Harga\n");

    for (i = 0; i < jumlahHistory; i++) {
        printf("%d. | %s | %d | Rp %.2f\n",
               i + 1, history[i].kode,
               history[i].jumlah, history[i].total);
    }
}

/* Menu 4: menghapus history berdasarkan nomor yang ditampilkan. */
void deleteHistory(void) {
    int nomor;
    int i;

    viewHistory();

    if (jumlahHistory == 0) {
        return;
    }

    printf("Masukkan nomor history yang ingin dihapus: ");
    nomor = bacaAngka();

    if (nomor < 1 || nomor > jumlahHistory) {
        printf("Nomor tidak valid.\n");
        return;
    }

    for (i = nomor - 1; i < jumlahHistory - 1; i++) {
        history[i] = history[i + 1];
    }

    jumlahHistory--;
    printf("Data Successfully delete..\n");
}

/* Menu 5: menghapus buku berdasarkan nomor yang ditampilkan. */
void deleteBuku(void) {
    int nomor;
    int i;

    viewBuku();

    if (jumlahBuku == 0) {
        return;
    }

    printf("Masukkan nomor buku yang ingin dihapus: ");
    nomor = bacaAngka();

    if (nomor < 1 || nomor > jumlahBuku) {
        printf("Nomor tidak valid.\n");
        return;
    }

    for (i = nomor - 1; i < jumlahBuku - 1; i++) {
        buku[i] = buku[i + 1];
    }

    jumlahBuku--;
    printf("Data Successfully delete..\n");
}

/* Menu 7: input transaksi, mencari harga buku, lalu menghitung total. */
void inputTransaksi(void) {
    char kode[20];
    int i;
    int indeks = -1;
    int jumlah;

    if (jumlahBuku == 0) {
        printf("Belum ada data buku. Tambahkan buku terlebih dahulu.\n");
        return;
    }

    if (jumlahHistory >= MAX_HISTORY) {
        printf("Penyimpanan history penuh.\n");
        return;
    }

    viewBuku();

    printf("Masukkan kode buku: ");
    if (!bacaTeks(kode, sizeof(kode))) {
        return;
    }

    for (i = 0; i < jumlahBuku; i++) {
        if (strcmp(kode, buku[i].kode) == 0) {
            indeks = i;
            break;
        }
    }

    if (indeks == -1) {
        printf("Kode buku tidak ditemukan.\n");
        return;
    }

    printf("Masukkan jumlah terjual: ");
    jumlah = bacaAngka();

    if (jumlah == -1) {
        return;
    }

    strcpy(history[jumlahHistory].kode, kode);
    history[jumlahHistory].jumlah = jumlah;
    history[jumlahHistory].total = jumlah * buku[indeks].harga;
    jumlahHistory++;

    if (simpanHistory()) {
        printf("Transaksi berhasil dicatat.\n");
        printf("Total harga: Rp %.2f\n",
               history[jumlahHistory - 1].total);
    } else {
        printf("Transaksi tersimpan di memori, tetapi gagal disimpan ke file.\n");
    }
}


/* Menampilkan data buku setelah proses sorting. */
void tampilkanHasilSort(void) {
    int i;

    printf("\n========== HASIL SORTING BUKU ==========\n");
    printf("No. | Kode Buku | Nama Buku | Jenis Buku | Harga\n");

    for (i = 0; i < jumlahBuku; i++) {
        printf("%d. | %s | %s | %s | Rp %.2f\n",
               i + 1, buku[i].kode, buku[i].nama,
               buku[i].jenis, buku[i].harga);
    }
}

/* Bubble Sort: mengurutkan nama buku secara ascending (A-Z). */
void bubbleSortNama(void) {
    int i, j;
    Buku sementara;

    for (i = 0; i < jumlahBuku - 1; i++) {
        for (j = 0; j < jumlahBuku - 1 - i; j++) {
            if (strcmp(buku[j].nama, buku[j + 1].nama) > 0) {
                sementara = buku[j];
                buku[j] = buku[j + 1];
                buku[j + 1] = sementara;
            }
        }
    }
}

/* Selection Sort: mengurutkan harga buku secara descending (terbesar-terkecil). */
void selectionSortHarga(void) {
    int i, j, indeksMaks;
    Buku sementara;

    for (i = 0; i < jumlahBuku - 1; i++) {
        indeksMaks = i;

        for (j = i + 1; j < jumlahBuku; j++) {
            if (buku[j].harga > buku[indeksMaks].harga) {
                indeksMaks = j;
            }
        }

        if (indeksMaks != i) {
            sementara = buku[i];
            buku[i] = buku[indeksMaks];
            buku[indeksMaks] = sementara;
        }
    }
}

/* Menu 8: pilihan pengurutan data buku. */
void menuSortBuku(void) {
    int pilihan;

    if (jumlahBuku == 0) {
        printf("Tidak ada data buku untuk diurutkan.\n");
        return;
    }

    do {
        printf("\n========== SORT BUKU ==========\n");
        printf("1. Urutkan berdasarkan nama (A-Z) - Bubble Sort\n");
        printf("2. Urutkan berdasarkan harga (tertinggi-terendah) - Selection Sort\n");
        printf("3. Kembali ke Menu Utama\n");
        printf("Pilihan: ");

        pilihan = bacaAngka();

        switch (pilihan) {
            case 1:
                bubbleSortNama();
                printf("Data buku berhasil diurutkan berdasarkan nama (A-Z).\n");
                tampilkanHasilSort();
                break;

            case 2:
                selectionSortHarga();
                printf("Data buku berhasil diurutkan berdasarkan harga (tertinggi-terendah).\n");
                tampilkanHasilSort();
                break;

            case 3:
                printf("Kembali ke menu utama.\n");
                break;

            case -1:
                return;

            default:
                printf("Pilihan tidak valid. Silakan pilih 1 sampai 3.\n");
        }
    } while (pilihan != 3);
}

int main(void) {
    int pilihan;
    int bukuTersimpan;
    int historyTersimpan;

    /* Membaca file saat program pertama kali dijalankan. */
    bacaFile();

    do {
        printf("\n===== TOKO BUKU LITERASI NUSANTARA JAYA =====\n");
        printf("1. Input Data Buku Baru\n");
        printf("2. View History Transaksi Penjualan\n");
        printf("3. View Buku\n");
        printf("4. Delete History\n");
        printf("5. Delete Buku\n");
        printf("6. Exit\n");
        printf("7. Input Transaksi Penjualan\n");
        printf("8. Sort Buku\n");
        printf("Pilihan: ");

        pilihan = bacaAngka();

        switch (pilihan) {
            case 1:
                inputBuku();
                break;

            case 2:
                viewHistory();
                break;

            case 3:
                viewBuku();
                break;

            case 4:
                deleteHistory();
                break;

            case 5:
                deleteBuku();
                break;

            case 6:
                bukuTersimpan = simpanBuku();
                historyTersimpan = simpanHistory();

                if (bukuTersimpan && historyTersimpan) {
                    printf("Data berhasil disimpan. Program selesai.\n");
                    return 0;
                }

                printf("Gagal menyimpan data. Periksa folder program.\n");
                break;

            case 7:
                inputTransaksi();
                break;

            case 8:
                menuSortBuku();
                break;

            case -1:
                printf("\nInput berakhir. Program dihentikan.\n");
                return 0;

            default:
                printf("Pilihan tidak valid. Silakan pilih menu 1 sampai 8.\n");
        }

    } while (1);

    return 0;
}
