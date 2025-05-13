cat << 'EOF' > fix_webserver.patch
--- a/src/webserver.c
+++ b/src/webserver.c
@@ -210,9 +210,17 @@ void handle_web_who_request( WWW_DESC *wdesc )
-  send_buf(wdesc->fd,"<HTML><HEAD><TITLE>Times Of Chaos Who List</TITLE></HEAD>
-");
-  send_buf(wdesc->fd,"<BODY BGCOLOR=\"#FFFFFF\"><B>Times Of Chaos Who List</B><P>
-");
-  for (d = descriptor_list; d; d = d->next)
+  /* send HTML header and open body */
+  send_buf(wdesc->fd,
+    "<HTML><HEAD><TITLE>Times Of Chaos Who List</TITLE></HEAD>\n"
+    "<BODY BGCOLOR=\\\"#FFFFFF\\\"><B>Times Of Chaos Who List</B><P>\n");
+
+  /* now list all playing descriptors */
+  for (d = descriptor_list; d; d = d->next)
   {
     if (d->character && d->connected == CON_PLAYING)
     {
@@ -220,6 +228,8 @@ void handle_web_who_request( WWW_DESC *wdesc )
       count++;
     }
   }
+  /* close it out */
+  send_buf(wdesc->fd,"</BODY></HTML>\n");
 }
EOF

patch -p1 < fix_webserver.patch
make clean
make
