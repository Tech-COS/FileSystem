section .note.GNU-stack noalloc noexec nowrite progbits
section .text
global read_byte
global detect_drive

; Sélection du disque maître (pour utilisation en 48-bit LBA)
select_master_drive:
    mov al, 0xA0
    mov dx, 0x1F6
    out dx, al ; Select Master.

    xor al, al
    dec dx
    out dx, al  ; 0x1F5, LBAhigh Init

    dec dx
    out dx, al  ; 0x1F4, LBAmid Init

    dec dx
    out dx, al  ; 0x1F3, LBAlo Init

    dec dx
    out dx, al  ; 0x1F2, Sector Count Init

    mov al, 0xEC
    mov dx, 0x1F7
    out dx, al ; IDENTIFY Command (0x1F7 is the Command Port in write mode and Status port in Read mode)

    in al, dx
    test al, al
    jz .smd_error
    ret

.smd_error:
    mov al, 0x1
    ret

check_non_ata_status_ports:
    mov dx, 0x1F4
    in al, dx           ; Checks 0x1F4 port to detect whether or not the drive is an ATA Drive
    test al, al
    jnz .cnasp_error

    inc dx
    in al, dx           ; Checks 0x1F5 port to detect whether or not the drive is an ATA Drive
    test al, al
    jnz .cnasp_error
    ret
.cnasp_error:
    mov al, 0x1
    ret

wait_for_information:
    mov dx, 0x1F7
.wait:
    in al, dx
    test al, 0b1         ; If ERR bit is set, the drive can't be read or written to.
    jnz .wfi_error
    test al, 0b1000      ; If DRQ value is set, the drive is ready and the Master Data Port 0x1F0 contains 256 values.
    jz .wait
    ret
.wfi_error:
    mov al, 0x1
    ret

detect_drive:
    pushfq
    push rdx
    push rcx
    push rbx
    push rax

    xor rax, rax
    xor rcx, rcx
    mov rbx, 1
    xor rdx, rdx
    call select_master_drive
    cmp al, 0x1
    je .error

    call wait_for_ready

    call check_non_ata_status_ports
    cmp al, 0x1
    je .error

    call wait_for_information
    cmp al, 0x1
    je .error

    mov rbx, 1
    call read_loop
.error:
    pop rax
    pop rbx
    pop rcx
    pop rdx
    popfq
    ret

wait_for_ready:
    mov dx, 0x1F7
    in al, dx
    in al, dx
    in al, dx
    in al, dx
.wait:
    in al, dx           ; Read the status register into AL.
    test al, 0x80       ; Check the busy bit (bit 7) to see if the drive is busy.
    jnz .wait           ; If the busy bit is set (non-zero), jump back to .wait and loop until the drive is ready.
    test al, 0x40
    jz .wait
    ret                 ; Return from the function once the drive is ready.

wait_for_ready_next:
    mov dx, 0x1F7
    in al, dx
    in al, dx
    in al, dx
    in al, dx
.wait:
    in al, dx           ; Read the status register into AL.
    test al, 0x80       ; Check the busy bit (bit 7) to see if the drive is busy.
    jnz .wait           ; If the busy bit is set (non-zero), jump back to .wait and loop until the drive is ready.
    test al, 0x8
    jz .wait

read_loop:
    mov dx, 0x1F0
    mov rcx, 256
.loop:
    in ax, dx              ; Lit un mot (16 bits) depuis le disque
    mov [rdi], ax          ; Stocke le mot dans le buffer
    add rdi, 2             ; Incrémente le pointeur de buffer
    loop .loop

    dec rbx                ; Move to the next sector
    test rbx, rbx
    jne wait_for_ready_next
    ret

; 48bit LBAs == 6 bytes.
; 16bit sectorcount == 2 bytes.
; A sectorcount of 0 == uint16 MAX == 32MB
; Each LBA number corresponds to the beginning of a 512 byte block.
; A 512 byte block is called a sector.
read_byte:
    pushfq
    push rdi
    push rsi
    push rdx
    push rcx
    push rbx
    push rax

    xor rax, rax
    mov rbx, rdx
    ; Select Master Drive
    mov dx, 0x1F6
    mov al, 0x40
    out dx, al

    ; Send the sectorcount high bits to port 0x1F2
    mov dx, 0x1F2
    mov al, bh
    out dx, al

    ; Send LBA byte 4 to 0x1F3:
    ror rsi, 24
    inc dx
    mov al, sil
    out dx, al

    ; Send LBA byte 5 to 0x1F4:
    ror rsi, 8
    inc dx
    mov al, sil
    out dx, al

    ; Send LBA byte 6 to 0x1F5:
    ror rsi, 8
    inc dx
    mov al, sil
    out dx, al

    rol rsi, 40

    ; Send the sectorcount low bits to port 0x1F2
    mov dx, 0x1F2
    mov al, bl
    out dx, al

    ; Send LBA byte 1 to 0x1F3
    inc dx
    mov al, sil
    out dx, al

    ; Send LBA byte 2 to 0x1F4
    shr rsi, 8
    inc dx
    mov al, sil
    out dx, al

    ; Send LBA byte 3 to 0x1F5
    shr rsi, 8
    inc dx
    mov al, sil
    out dx, al

    ; Send the "READ SECTORS EXT" command (0x24) to port 0x1F7
    mov dx, 0x1F7
    mov al, 0x24
    out dx, al

    mov dx, 0x1F1

    ; Wait until the disc is ready
    call wait_for_ready


    ; Read Data
    call read_loop

    pop rax
    pop rbx
    pop rcx
    pop rdx
    pop rsi
    pop rdi
    popfq
    ret
