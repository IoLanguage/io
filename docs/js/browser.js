String.prototype.interpolate = function (obj) {
  if(!obj)
    return this;

  return this.replace(/#{(\w+)}/g, function (match, key) {
    return obj[key] || "";
  });
};

var DocsBrowser = {
  currentCategory:  null,
  currentAddon:     null,
  currentProto:     null,
  currentSlot:      null,
  collapsed:        false,

  init: function () {
    this.loadDocs();
  },
  loadDocs: function () {
    var self = this;
    $.getJSON("docs.json", function (docs) {
      if(!docs)
        return false;

      self.docs = docs;
      self.createIndex();
      self.attachEvents();
      self.listCategories();
    });
  },
  _pathsIndex: [],
  _protosIndex: {},
  createIndex: function () {
    for(var category in this.docs)
      for(var addon in this.docs[category])
        for(var proto in this.docs[category][addon]) {
          this._protosIndex[proto] = [category, addon, proto];
          for(var slot in this.docs[category][addon][proto].slots)
            this._pathsIndex.push([category, addon, proto, slot]);
        }
  },
  monitorHash: function () {
    var self = DocsBrowser;

    if(self._last_hash === window.location.hash)
      return;

    var hash = window.location.hash.split("/").slice(1);
    if(hash.length == 0)
      return;

    if(self.currentCategory !== hash[0])
      self.listAddons(hash[0]);
    if(hash.length >= 1 && self.currentAddon !== hash[1])
      self.listProtos(hash[1]);
    if(hash.length > 2 && self.currentProto !== hash[2])
      self.listSlots(hash[2]);
    if(hash.length > 3)
      self.showSlot(hash[3]);

    self._last_hash = window.location.hash;
  },
  attachEvents: function () {
    var self = this;

    $("h1")
      .before('<div id="searchResults" style="display:none"></div>')
      .before('<input type="search" id="search" placeholder="Search..." title="Search..."/>');
    $("body")
      .append('<p id="protoDescription"></p>')
      .append('<div id="protoSlots"></div>');
    $("#categories").parent()
      .append('<td id="addons" valign="top" class="column">')
      .append('<td id="protos" valign="top" class="column">')
      .append('<td id="slots"  valign="top" class="column">');

    setInterval(this.monitorHash, 200);

    $("#searchResults").click(function () {
      $(this).empty().hide();
      $("#search")[0].value = "";
    });

    $("#search").keyup(function (e) {
      // Enter key
      if(e.keyCode === 13) {
        var link = $("#searchResults a:first").attr("href");
        if(link) {
          window.location.hash = link;
          $("#searchResults").click();
          this.value = "";
        }
      } else if(this.value.length) {
        self.search(this.value);
      } else {
        $("#searchResults").empty().hide();
      }
    });
    
    $(window).keyup(function (e) {
      if(e.target.nodeName.toLowerCase() === "html"
        && String.fromCharCode(e.keyCode).toLowerCase() === "f")
        $("#search")[0].focus();
    });
  },

  listCategories: function () {
    var $categories = $("#categories");
    this.appendItemsToList($categories, this.docs, "linkToCategory");
    $("#categories-column").prepend($categories);
  },
  listAddons: function (category) {
    var $addons = $("#addons"),
        addons = this.docs[category];
    this.currentCategory = category;
    this.curentAddon = null;
    this.currentProto = null;
    this.currentSlot = null;
    this.highlightMenuItem(category + "_category");

    this.appendItemsToList($addons, addons, "linkToAddon");
    $("#protos").empty();
    $("#slots").empty();
    $("#protoDescription").empty();
    $("#protoSlots").empty();
    $addons.insertAfter("#categories");

    if(addons[category] && window.location.hash.split("/").length == 2) {
      window.location = window.location + "/" + category;
      this.listProtos(category);
    }
  },
  listProtos: function (addon) {
    if(!addon)
      return false;

    var $protos = $("#protos"),
        protos = this.docs[this.currentCategory][addon];
    this.currentAddon = addon;
    this.highlightMenuItem(addon + "_addon");

    this.appendItemsToList($protos, protos, "linkToProto");
    $("#slots").empty();
    $("#protoDescription").empty();
    $("#protoSlots").empty();
    $protos.insertAfter("#addons");
    
    if(protos[addon] && window.location.hash.split("/").length == 3) {
      window.location = window.location + "/" + addon;
      this.listSlots(addon);
    }
  },
  listSlots: function (proto) {
    var $slots      = $("#slots"),
        $descriptions = $("#protoSlots"),
        _proto      = this.docs[this.currentCategory][this.currentAddon][proto],
        slots       = _proto.slots,
        description = _proto.description,
        html_description = "<a name=\"/#{aName}\"></a><b>#{title}</b><p><div class=\"slotDescription\">#{body}</div></p>";
    this.currentProto = proto;
    this.highlightMenuItem(proto + "_proto");

    var keys = [], n = 0;
    for(var slot in slots)
      keys.push(slot);

    keys = keys.sort().reverse();
    n = keys.length;
    $slots.detach().empty();
    $descriptions.detach().empty();

    while(n--) {
      var slot = keys[n];
      $slots.append(this.linkToSlot(slot));
      $descriptions.append(html_description.interpolate({
        aName: this.aNameForHash(slot),
        title: this.parseSlotName(slot),
        body: this.parseSlotDescription(slots[slot])
      }));
    }

    $("#protoDescription").html(this.parseSlotDescription(description) || "");
    
    $slots.insertAfter("#protos");
    $descriptions.appendTo("body");
  },
  showSlot: function (slot) {
    // Some nice scrolling effect?
    //$("a[name=" + window.location.hash.slice(1) + "]").next();
  },

  _searchCache: {},
  search: function (term) {
    if(this._searchCache[term])
      return this.displaySearch(term, this._searchCache[term]);

    var results = [],
      mode = this.getSearchMode(term);

    switch(mode) {
      case "currentProto":
        var scope = this.docs[this.currentCategory][this.currentAddon][this.currentProto].slots,
            _term = $.trim(term);

          for(var slot in scope)
            if(slot.indexOf(_term) !== -1)
              results.push([this.currentCategory, this.currentAddon, this.currentProto, slot]);
        break;

      case "givenProto":
        var __term = term.split(" "),
            _term = $.trim(__term[1]),
            ca = this.getPathToProto(__term[0]);
        if(!ca)
          break;

        var scope = this.docs[ca[0]][ca[1]][__term[0]].slots;
        for(var slot in scope)
          if(slot.indexOf(_term) !== -1)
            results.push([ca[0], ca[1], __term[0], slot]);
        break;

      case "searchProto":
        var index = this._protosIndex;
        for(var proto in index)
          if(proto.indexOf(term) !== -1)
            results.push(index[proto]);

        break;

      case "allProtos":
        var index = this._pathsIndex, n = index.length;
        while(n--)
          if(index[n][3].indexOf(term) !== -1)
            results.push(index[n]);
        break;

      case "none":
      default:
        return false;
    }

    this._searchCache[term] = results;
    this.displaySearch(term, results);
  },
  getSearchMode: function (term) {
    var firstChar = term[0];

    if(term.length < 2)
      return "none";
    else if(firstChar === " ")
      return "currentProto";
    else if(firstChar == firstChar.toUpperCase())
      if(term.indexOf(" ") == -1)
        return "searchProto";
      else
        return "givenProto";
    else
      return "allProtos";

    return none;
  },
  getPathToProto: function (proto) {
    return this._protosIndex[proto] || false;
  },
  displaySearch: function (term, results) {
    var $results = $("#searchResults"),
        _term = $.trim(term),
        mode = this.getSearchMode(term),
        n = results.length;

    if(!n) {
      $results.empty().hide();
      return false;
    }

    if(mode === "givenProto")
      _term = _term.split(" ")[1];

    $results.detach().empty();
    while(n--) {
      var result = results[n],
          slot = (result[3] || result[2]).replace(_term, "<u>" + _term + "</u>"),
          data = {
            link: "#/" + result.join("/").split("(")[0],
            slot: slot,
            proto: result[2]
          };

      if(mode === "searchProto")
        data.proto = result.slice(0, -1).join(" ");

      data.proto = "<span>" + data.proto.split(" ").join("</span><span>") + "</span>";
      $results.prepend("<li><a href=\"#{link}\">#{proto} #{slot}</a></li>".interpolate(data));
    }

    $results.show().insertBefore("h1");
//    $results.show().css("top", $("#search").offset().top).appendTo("body");
  },

  createMenuLink: function (text, type, link) {
    arguments[2] = link.split("(")[0];
    return '<div id="#{0}_#{1}" class="type_#{1} indexItem"><a href="#/#{2}">#{0}</a></div>'.interpolate(arguments);
  },
  linkToCategory: function (category) {
    return this.createMenuLink(category, "category", category);
  },
  linkToAddon: function (addon) {
    return this.createMenuLink(addon, "addon", [this.currentCategory, addon].join("/"));
  },
  linkToProto: function (proto) {
    return this.createMenuLink(proto, "proto", [this.currentCategory, this.currentAddon, proto].join("/"));
  },
  linkToSlot: function (slot) {
    var url = this.aNameForHash(slot),
        text = this.parseSlotName(slot);
    return '<div id="#{0}_slot" class="type_slot indexItem"><a href="#/#{1}">#{2}</a></div>'.interpolate([slot, url, text]);
  },
  highlightMenuItem: function (id) {
    var $link = $(document.getElementById(id));
    $link.parent().find(".indexItemSelected").removeClass("indexItemSelected").addClass("indexItem");
    $link.addClass("indexItemSelected").removeClass("indexItem");
  },
  /*
  collapseMenu: function () {
    this.collapsed = true;
    $("#browser ol .indexItem").fadeOut("fast");
  },
  showMenu: function () {
    this.collapsed = false;
    $("#browser ol .indexItem").fadeIn("fast");
  }, */
  parseSlotName: function (slot) {
    return slot
      .replace(/\<(.+)\>/,  "&lt;$1&gt;")
      .replace(/\((.*)\)/,  "<span class=\"args\">($1)</span>")
      .replace(/\[(.+)\]/,  "<span class=\"optionalArgs\">$1</span>")
      .replace(/(\.{3})/,   "<span class=\"infiniteArgs\">&hellip;</span>");
  },
  parseSlotDescription: function (body) {
    var self = this;
    return body.replace(/\[\[(\w+) (\w+)\]\]/g, function (line, proto, slot) {
      var link = self.getPathToProto(proto);
      if(!link)
        return '<code>#{1} #{2}</code>'.interpolate(arguments);

      link.push(proto, slot);
      return '<a href="#{0}"><code>#{1} #{2}</code></a>'.interpolate([self.pathToHash(link), proto, slot]);
    }).replace(/\[\[(\w+)\]\]/g, function (line, proto) {
      var link = self.getPathToProto(proto);
      if(!link)
        return '<code>' + proto + '</code>';

      return '<a href="#{0}"><code>#{1}</code></a>'.interpolate([self.pathToHash(link), proto]);
    });
  },
  pathToHash: function (path) {
    return "#/" + path.join("/");
  },
  aNameForHash: function (slot) {
    slot = slot ? "/" + slot.split("(")[0] : "";
    return window.location.hash.split("(")[0].split("/").slice(1, 4).join("/") + slot;
  },
  appendItemsToList: function($list, items, linkToFn) {
    $list.detach().empty();
    var keys = [],
        makeLink = this[linkToFn],
        n;

    for(var item in items)
      keys.push(item);

    n = keys.length;
    keys = keys.sort().reverse();
    while(n--)
      $list.append(this[linkToFn](keys[n]));

    return $list;
  }
};

$(function() { DocsBrowser.init(); })
