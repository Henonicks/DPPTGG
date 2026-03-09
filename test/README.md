To test, create a `config.json` in the `test` directory. The content is as follows:

```json
{
    "TOPGG_WEBHOOK_LISTEN_IP" : "X.X.X.X",
    "TOPGG_WEBHOOK_LISTEN_PORT" : 12345,
    "TOPGG_WEBHOOK_SECRETS" : [
        {"endpoint1" : "secret1"},
        {"endpoint2" : "secret2"}
    ],
    "TOPGG_BOT_TOKEN" : "bot token on top.gg",
    "USER_ID" : 123456789012345678,
    "BOT_ID" : 123456789012345678
}
```
