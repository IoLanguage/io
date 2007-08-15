
blog := HDB clone setPath("blog.hdb") open root
posts := blog _at("posts")

// add a post with TITLE and CONTENT

newPost := posts _at(Date clone now asString)
newPost _at("title") setData(TITLE)
newPost _at("content") setData(CONTENT)

// print comments for blog post POSTKEY

posts at(POSTKEY) _at("comments") foreach(comment,
    writeln(
        comment at("user") data, " on ", 
        comment at("date") data, " writes:",
        comment at("content") data, "\n"
    )
)

// remove a post for POSTKEY - removes all comments and subfolders

posts at(POSTKEY) remove

