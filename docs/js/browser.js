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
    this.attachEvents();

    setInterval(this.monitorHash, 200);
  },
  loadDocs: function () {
    var self = this;
    $.getJSON("reference/docs.json", function (docs) {
      self.docs = docs;
      self.listCategories();
    });
  },
  monitorHash: function () {
    var self = DocsBrowser;

    if(self._last_hash == window.location.hash)
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
    /* $("#browser").mouseenter(function () {
      if(self.collapsed) self.showMenu();
    }); */
    $("#searchResults").click(function () {
      $(this).empty().hide();
    });
    $("#search").keyup(function (e) {
      if(e.keyCode == 13) {
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
  },

  listCategories: function () {
    var $categories = $("#categories");
    this.appendItemsToList($categories, this.docs, "linkToCategory");
    $("#browser").prepend($categories);
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
    $addons.insertAfter("#categories");

    if(addons.hasOwnProperty(category))
      window.location.hash = window.location.hash + "/" + category;
  },
  listProtos: function (addon) {
    var $protos = $("#protos"),
        protos = this.docs[this.currentCategory][addon];
    this.currentAddon = addon;
    this.highlightMenuItem(addon + "_addon");

    this.appendItemsToList($protos, protos, "linkToProto");
    $("#slots").empty();
    $protos.insertAfter("#addons");
  },
  listSlots: function (proto) {
    var $slots      = $("#slots"),
        $descriptions = $("#slotDescriptions"),
        _proto      = this.docs[this.currentCategory][this.currentAddon][proto],
        slots       = _proto.slots,
        description = _proto.description,
        html_description = "<div><a name=\"/#{aName}\"></a><h4>#{title}</h4><div class=\"slotDescription\">#{body}</div></div>";
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

    $slots.width($("body").width() - ($("#protos").offset().left + $("#protos").width()));
    if(description)
      $slots.append("<li class=\"description indexItem\">" + description + "</li>");

    $slots.insertAfter("#protos");
    $descriptions.insertAfter("#header");
  },
  showSlot: function (slot) {
    // Some nice effect?
    //$("a[name=" + window.location.hash.slice(1) + "]").next();
  },

  _searchCache: {},
  search: function (term) {
    if(this._searchCache[term])
      return this.displaySearch(term, this._searchCache[term]);

    var results = [],
      mode = this.getSearchMode(term),
      scope;

    switch(mode) {
      case "currentProto":
        scope = this.docs[this.currentCategory][this.currentAddon][this.currentProto].slots;
        var _term = $.trim(term);
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

        scope = this.docs[ca[0]][ca[1]][__term[0]].slots;
        for(var slot in scope)
          if(slot.indexOf(_term) !== -1)
            results.push([ca[0], ca[1], __term[0], slot]);
        break;

      case "searchProto":
        for(var category in this.docs)
          for(var addon in this.docs[category])
            for(var proto in this.docs[category][addon])
              if(proto.indexOf(term) !== -1)
                results.push([category, addon, proto]);
        break;

      case "allProtos":
        for(var category in this.docs)
          for(var addon in this.docs[category])
            for(var proto in this.docs[category][addon])
              for(var slot in this.docs[category][addon][proto].slots)
                if(slot.indexOf(term) !== -1)
                  results.push([category, addon, proto, slot]);
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
  _catCache: {},
  getPathToProto: function (proto) {
    if(this._catCache[proto])
      return this._catCache[proto];

    for(var category in this.docs)
      for(var addon in this.docs[category])
        if(this.docs[category][addon].hasOwnProperty(proto))
          return this._catCache[proto] = [category, addon];

    return false;
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

    $results.show().css($("#search").offset()).appendTo("body");
  },

  createMenuLink: function (text, type, link) {
    arguments[2] = link.split("(")[0];
    return '<li id="#{0}_#{1}" class="type_#{1} indexItem"><a href="#/#{2}">#{0}</a></li>'.interpolate(arguments);
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
    return '<li id="#{0}_slot" class="type_slot indexItem"><a href="#/#{1}">#{2}</a></li>'.interpolate([slot, url, text]);
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
      var link = self.getPathToProto(proto)
      link.push(proto, slot);
      return "<a href=\"#{0}\">#{1} #{2}</a>".interpolate([self.pathToHash(link), proto, slot]);
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
