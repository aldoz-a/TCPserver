package main

import (
    "bufio"
    "fmt"
    "net"
    "os"
)

func main() {
    // test argomenti
    if len(os.Args) != 2 {
        // errore di chiamata
        fmt.Printf("%s: numero argomenti errato\n", os.Args[0])
        fmt.Printf("uso: %s port [e.g.: %s 9999]\n", os.Args[0], os.Args[0])
        return
    }

    // start ascolto sul port richiesto
    port := ":" + os.Args[1]             // set port (i.e.: ":port")
    lner, err := net.Listen("tcp", port) // set listener con network di tipo TCP
    if err != nil {
        // errore di ascolto
        fmt.Println(err)
        return
    }

    defer lner.Close() // prenoto la chiusura del listener

    // accetta connessioni da un client entrante
    fmt.Printf("%s: attesa connessioni entranti...\n", os.Args[0])
    conn, err := lner.Accept()
    if err != nil {
        // errore di accept
        fmt.Println(err)
        return
    }

    // loop di ricezione messaggi dal client
    connrdr := bufio.NewReader(conn) // reader sulla connessione
    for {
        // attende la ricezione di un messaggio
        client_msg, err := connrdr.ReadString('\n') // leggo con il conn reader
        if err != nil {
            // errore di ricezione
            fmt.Println(err)
            return
        }

        // mostra il messaggio ricevuto e compone la risposta
        fmt.Printf("%s: ricevuto messaggio: %s", os.Args[0], string(client_msg))
        server_msg := fmt.Sprintf("mi hai scritto %s", string(client_msg))

        // send messaggio di ritorno al client
        _, err = conn.Write([]byte(server_msg)) // scrivo sulla connessione
        if err != nil {
            // errore di send
            fmt.Println(err)
            return
        }
    }
}
