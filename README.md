# JsonEditor
For my website, my projects are stored in the JSON format. I was already tired after writing two in my favourite editor Spacemacs :heart: so... I built this editor with the help from QT.

## Features
It can open my data.json and display my projects in a list.

I can click on a project to edit it or click a button named 'new' to create a new project.

If i edit a card and exit, it will ask if I'd like to save or exit. If i've edited a project and saved it and then try to exit, I am asked if a i do not care about my edits.
So yeah, editing a card and editing the whole file is seperate things in this editor, found it easier with the "Are you sure you want to exit when you have ..."-messages.


## data.json format
Standard JSON format. You never saw the old format, but in this new improved format, the different contents of the card is stored in order, under the '_content' key. Example follows:

```json
[
    {
        "_id": 0,
        "_title": "The Debug Card",
        "_description": "Testing images",
        "_headerhref": "default.png",
        "_headeralt": "Debug Card!",
        "_content": [
            "p", "Debug card.",
            "p", "So debuggy.",
            "p", "an image is coming",
            "img", {
                "alt": "describing image",
                "href": "default.png",
                "image_text": "some image text. Cool."
            },
            "li", {
                "alt": "a link",
                "href": "example.com"
            },
            "p", "this is aftter an image",
            "p", "cool it did not break"
        ]
    }
]
```
