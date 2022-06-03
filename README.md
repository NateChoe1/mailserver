# Simple docker mailserver configuration

## What is this

This is just a simple [docker mailserver](https://github.com/docker-mailserver/docker-mailserver)
configuration so that I don't have to rebuild it from scratch if something goes wrong.

## How do I use this?

First, run

```
sh setup.sh
```

Unfortunately, git doesn't allow you to create empty directories so that script just creates a few
for your convenience. Then, set up the DNS records. Mine look like this, although yours will
obviously have a different URL and IP address.

```
natechoe.dev	A	3600	45.33.6.152
; Obviously, you don't own natechoe.dev, and your IP address is not 45.33.6.152,
; just set those to your domain and your IP address
natechoe.dev	MX	3600	0 mail.natechoe.dev
mail.natechoe.dev	CNAME	3600	natechoe.dev
```

Then, set up a reverse-DNS lookup for your IP address to your domain. This can only be done through
your VPS provider or ISP. In Linode, go to the Network tab in your Linode, and click "Edit RDNS" by
your IP address.

Next, run the following command:

```
docker-container up mailserver
```

You should see a lot of logs from the mail server. This phase is just for debugging, so we really
need these logs. After that, open a new terminal while keeping the old one open and run this
command

```
docker exec mailserver setup email add [email] [password]
```

This command creates a new email address with the email and password specified in the command. If
you don't get any errors, then congratulations, you're nearly there.

**BEGIN OPTIONAL SECTION**

This step is technically optional, but if you skip it your emails will almost definitely be marked
as spam.

Run

```
docker exec mailserver setup config dkim
```

Check the contents of `docker-data/dms/config/opendkim/keys/[your url]/mail.txt` and add that to
your DNS record. For example, my file says

```
mail._domainkey	IN	TXT	( "v=DKIM1; h=sha256; k=rsa; "
	  "p=MIICIjANBgkqhkiG9w0BAQEFAAOCAg8AMIICCgKCAgEAncKNo62sftY7hPc2hyVc2tebUI38RTy4w5WUa7aKXf/Qr4eeki0/K9yB1wPDg97TTrVIbCOCohWY9/d77mC8KaCxcRvoV1tZRacA3ZHVqseZ+N8KD4ImvIwVNo1zmsZwzce5mcTO9a32aEfPbEmMCd07ozLY1mJaSi2utxIdahxDZIuF+PpHojK5y1NNKe6iesx4eutNH7/FhJ"
	  "O+bPRdcJi2b727C1LGIoIVZIy7eqSFRZZ6p47UCrYn6/YLKEW1JdVl/SC/+HDyO6DeUlC/i/ebzrLqIuY7ZSnK408o3dxySpuZ1KSiIfG0efvLCQkNP/eYw4qLi5Nq7rGKynpTUDAbNP58pjbKv/4BvzMdDEUUOlqMFzrbZXZROU/FsyJIr+YKwaMDUjIeXR/PhH3zue/I+8oNoMEaL8n1UynlLapTvaTnDbOOcMP2qi41rb2vDpmZTgxe"
	  "+rx93DoipjLb/dpsf0xM0px68BrmThW8X0l15W2pBu3Uytg5cpTkntGOM0psGx9TMX4vrY8aki4/rbSrA+9FNm8GCylWJHyJXB1Y49DWQdIxY/8ewkMdbOqRHjIsjTvG6M4owI6NiGI0RIi5oZMRYomHndvZyzlNtgCCKAVABaIYbi0QHXWzyglbgKZPmXC+Afw/3NeIyFoTTUfjjJKHXq5U7fs1wR7B9MsCAwEAAQ==" )  ; ----- DKIM key mail for natechoe.dev
```

so my DNS record says

```
mail._domainkey.natechoe.dev	TXT	3600	"v=DKIM1; h=sha256; k=rsa; p=MIICIjANBgkqhkiG9w0BAQEFAAOCAg8AMIICCgKCAgEAncKNo62sftY7hPc2hyVc2tebUI38RTy4w5WUa7aKXf/Qr4eeki0/K9yB1wPDg97TTrVIbCOCohWY9/d77mC8KaCxcRvoV1tZRacA3ZHVqseZ+N8KD4ImvIwVNo1zmsZwzce5mcTO9a32aEfPbEmMCd07ozLY1mJaSi2utxIdahxDZIuF+PpHojK5y1NNKe6iesx4eutNH7/FhJO+bPRdcJi2b727C1LGIoIVZIy7eqSFRZZ6p47UCrYn6/YLKEW1JdVl/SC/+HDyO6DeUlC/i/ebzrLqIuY7ZSnK408o3dxySpuZ1KSiIfG0efvLCQkNP/eYw4qLi5Nq7rGKynpTUDAbNP58pjbKv/4BvzMdDEUUOlqMFzrbZXZROU/FsyJIr+YKwaMDUjIeXR/PhH3zue/I+8oNoMEaL8n1UynlLapTvaTnDbOOcMP2qi41rb2vDpmZTgxe+rx93DoipjLb/dpsf0xM0px68BrmThW8X0l15W2pBu3Uytg5cpTkntGOM0psGx9TMX4vrY8aki4/rbSrA+9FNm8GCylWJHyJXB1Y49DWQdIxY/8ewkMdbOqRHjIsjTvG6M4owI6NiGI0RIi5oZMRYomHndvZyzlNtgCCKAVABaIYbi0QHXWzyglbgKZPmXC+Afw/3NeIyFoTTUfjjJKHXq5U7fs1wR7B9MsCAwEAAQ=="
```

**END OPTIONAL SECTION**

Finally, run the following command

```
docker-container up -d mailserver
```

This should run your mail server without error (hopefully)
