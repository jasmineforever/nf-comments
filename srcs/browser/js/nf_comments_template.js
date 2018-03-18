function template(locals) {
var buf = [];
var jade_mixins = {};
var jade_interp;
;var locals_for_with = (locals || {});(function (comments_array, undefined) {
var char_num_max = 255
var placeholder = {"name": "名字（可选）", "email": "邮箱（可选）", "website": "网站（可选）", "content": "说点什么吧"}
var reply_text = "提交"
jade_mixins["add_name_show"] = jade_interp = function(val){
var block = (this && this.block), attributes = (this && this.attributes) || {};
var my_class = "nf-comment-name"
var my_type = "span"
{
if ( val.is_owner)
{
var my_class = my_class + ' nf-owner-name'
}
if ( val.website != '')
{
var my_class = my_class + ' nf-user-website-link'
var my_type = "a"
}
if ( my_type == "a")
{
buf.push("<" + (my_type) + (jade.attr("href", '' + (val.website) + '', true, false)) + " target=\"_blank\"" + (jade.cls(['' + (my_class) + ''], [true])) + ">" + (jade.escape((jade_interp = val.name) == null ? '' : jade_interp)) + "</" + (my_type) + ">");
}
else
{
buf.push("<" + (my_type) + (jade.cls(['' + (my_class) + ''], [true])) + ">" + (jade.escape((jade_interp = val.name) == null ? '' : jade_interp)) + "</" + (my_type) + ">");
}
}
};
jade_mixins["add_comments"] = jade_interp = function(val, suffix1, suffix2){
var block = (this && this.block), attributes = (this && this.attributes) || {};
buf.push("<div" + (jade.attr("id", 'nf_avatar_' + (suffix1) + '', true, false)) + " class=\"nf-avatar-frame\"></div><div class=\"nf-v-box\"><div class=\"nf-name-title\">");
jade_mixins["add_name_show"](val);
if ( val.reply_to != '')
{
buf.push("<span class=\"nf-comment-name\"> 回复 " + (jade.escape((jade_interp = val.reply_to) == null ? '' : jade_interp)) + "</span>");
}
if ( 1)
{
buf.push("<span class=\"nf-comment-name-spacer\"> • </span><span class=\"nf-comment-time\">" + (jade.escape((jade_interp = val.time) == null ? '' : jade_interp)) + "</span>");
}
buf.push("</div><div class=\"nf-comment-content\">" + (jade.escape((jade_interp = val.content) == null ? '' : jade_interp)) + "</div><div" + (jade.attr("id", 'nf_reply_button_' + (suffix1) + '', true, false)) + " class=\"nf-reply-button\">回复</div><div" + (jade.attr("id", 'nf_comment_area_' + (suffix1) + '', true, false)) + " class=\"nf-comment-area\"><div" + (jade.attr("id", 'nf_comment_input_frame_' + (suffix1) + '', true, false)) + " class=\"nf-comment-input-frame\"><textarea" + (jade.attr("id", 'nf_content_input_' + (suffix1) + '', true, false)) + " rows=\"3\"" + (jade.attr("maxlength", '' + (char_num_max) + '', true, false)) + (jade.attr("placeholder", '' + (placeholder.content) + '', true, false)) + " class=\"nf-content-input\"></textarea><div class=\"nf-content-input-count\"><label" + (jade.attr("id", 'nf_content_input_count_' + (suffix1) + '', true, false)) + ">0</label><label> / " + (jade.escape((jade_interp = char_num_max) == null ? '' : jade_interp)) + "</label></div></div><div" + (jade.attr("id", 'nf_inline_form_' + (suffix1) + '', true, false)) + " class=\"nf-inline-form\"><div class=\"nf-info-input-group nf-h-box\"><div class=\"glyphicon glyphicon-user nf-info-button\"></div><input" + (jade.attr("id", 'nf_name_input_' + (suffix1) + '', true, false)) + " type=\"text\"" + (jade.attr("placeholder", "" + (placeholder.name) + "", true, false)) + " class=\"nf-info-input nf-info-name-input\"/></div><div class=\"nf-info-input-group nf-h-box\"><div class=\"glyphicon glyphicon-envelope nf-info-button\"></div><input" + (jade.attr("id", 'nf_email_input_' + (suffix1) + '', true, false)) + " type=\"text\"" + (jade.attr("placeholder", "" + (placeholder.email) + "", true, false)) + " class=\"nf-info-input nf-info-email-input\"/></div><div class=\"nf-info-input-group nf-h-box\"><div class=\"glyphicon glyphicon-link nf-info-button\"></div><input" + (jade.attr("id", 'nf_website_input_' + (suffix1) + '', true, false)) + " type=\"text\"" + (jade.attr("placeholder", "" + (placeholder.website) + "", true, false)) + " class=\"nf-info-input nf-info-website-input\"/></div><div class=\"nf-remember-me nf-h-box\"><input type=\"checkbox\" class=\"nf-remember-me-checkbox\"/><label class=\"nf-remember-me-label\">记住我</label></div><div class=\"nf-submit-box\"><div" + (jade.attr("id", 'nf_submit_button_' + (suffix1) + '_' + (suffix2) + '', true, false)) + " type=\"submit\" class=\"nf-submit-button\">" + (jade.escape((jade_interp = reply_text) == null ? '' : jade_interp)) + "</div></div></div><div" + (jade.attr("id", 'nf_error_tips_' + (suffix1) + '', true, false)) + " class=\"nf-error-tips\"></div></div></div>");
};
buf.push("<div class=\"nf-comment-input-main\"><div id=\"nf-comment-input-frame-0\" class=\"nf-comment-input-frame\"><textarea id=\"nf_content_input_0\" rows=\"3\"" + (jade.attr("maxlength", "" + (char_num_max) + "", true, false)) + (jade.attr("placeholder", '' + (placeholder.content) + '', true, false)) + " class=\"nf-content-input\"></textarea><div class=\"nf-content-input-count\"><label id=\"nf_content_input_count_0\">0</label><label> / " + (jade.escape((jade_interp = char_num_max) == null ? '' : jade_interp)) + "</label></div></div><div id=\"nf-inline-form-0\" class=\"nf-inline-form\"><div class=\"nf-info-input-group nf-h-box\"><div class=\"glyphicon glyphicon-user nf-info-button\"></div><input id=\"nf_name_input_0\" type=\"text\"" + (jade.attr("placeholder", "" + (placeholder.name) + "", true, false)) + " class=\"nf-info-input nf-info-name-input\"/></div><div class=\"nf-info-input-group nf-h-box\"><div class=\"glyphicon glyphicon-envelope nf-info-button\"></div><input id=\"nf_email_input_0\" type=\"text\"" + (jade.attr("placeholder", "" + (placeholder.email) + "", true, false)) + " class=\"nf-info-input nf-info-email-input\"/></div><div class=\"nf-info-input-group nf-h-box\"><div class=\"glyphicon glyphicon-link nf-info-button\"></div><input id=\"nf_website_input_0\" type=\"text\"" + (jade.attr("placeholder", "" + (placeholder.website) + "", true, false)) + " class=\"nf-info-input nf-info-website-input\"/></div><div class=\"nf-remember-me nf-h-box\"><input type=\"checkbox\" id=\"nf_remember_me_checkbox_0\" class=\"nf-remember-me-checkbox\"/><label class=\"nf-remember-me-label\">记住我</label></div><div class=\"nf-submit-box\"><div id=\"nf_submit_button_0_0\" type=\"submit\" class=\"nf-submit-button\">" + (jade.escape((jade_interp = reply_text) == null ? '' : jade_interp)) + "</div></div></div><div id=\"nf_error_tips_0\" class=\"nf-error-tips\"></div></div><div class=\"nf-v-box nf-comments-main\">");
if ( comments_array.max_page > 0)
{
// iterate comments_array.comments
;(function(){
  var $$obj = comments_array.comments;
  if ('number' == typeof $$obj.length) {

    for (var $index = 0, $$l = $$obj.length; $index < $$l; $index++) {
      var val = $$obj[$index];

// iterate val
;(function(){
  var $$obj = val;
  if ('number' == typeof $$obj.length) {

    for (var index_sub = 0, $$l = $$obj.length; index_sub < $$l; index_sub++) {
      var val_sub = $$obj[index_sub];

if ( index_sub == 0)
{
buf.push("<div class=\"nf-h-box nf-comment-frame\">");
jade_mixins["add_comments"](val_sub, val_sub.comment_id, val[0].comment_id);
buf.push("</div>");
}
else
{
buf.push("<div class=\"nf-follow-box\"><div class=\"nf-h-box nf-follow-comment-frame\">");
jade_mixins["add_comments"](val_sub, val_sub.comment_id, val[0].comment_id);
buf.push("</div></div>");
}
    }

  } else {
    var $$l = 0;
    for (var index_sub in $$obj) {
      $$l++;      var val_sub = $$obj[index_sub];

if ( index_sub == 0)
{
buf.push("<div class=\"nf-h-box nf-comment-frame\">");
jade_mixins["add_comments"](val_sub, val_sub.comment_id, val[0].comment_id);
buf.push("</div>");
}
else
{
buf.push("<div class=\"nf-follow-box\"><div class=\"nf-h-box nf-follow-comment-frame\">");
jade_mixins["add_comments"](val_sub, val_sub.comment_id, val[0].comment_id);
buf.push("</div></div>");
}
    }

  }
}).call(this);

    }

  } else {
    var $$l = 0;
    for (var $index in $$obj) {
      $$l++;      var val = $$obj[$index];

// iterate val
;(function(){
  var $$obj = val;
  if ('number' == typeof $$obj.length) {

    for (var index_sub = 0, $$l = $$obj.length; index_sub < $$l; index_sub++) {
      var val_sub = $$obj[index_sub];

if ( index_sub == 0)
{
buf.push("<div class=\"nf-h-box nf-comment-frame\">");
jade_mixins["add_comments"](val_sub, val_sub.comment_id, val[0].comment_id);
buf.push("</div>");
}
else
{
buf.push("<div class=\"nf-follow-box\"><div class=\"nf-h-box nf-follow-comment-frame\">");
jade_mixins["add_comments"](val_sub, val_sub.comment_id, val[0].comment_id);
buf.push("</div></div>");
}
    }

  } else {
    var $$l = 0;
    for (var index_sub in $$obj) {
      $$l++;      var val_sub = $$obj[index_sub];

if ( index_sub == 0)
{
buf.push("<div class=\"nf-h-box nf-comment-frame\">");
jade_mixins["add_comments"](val_sub, val_sub.comment_id, val[0].comment_id);
buf.push("</div>");
}
else
{
buf.push("<div class=\"nf-follow-box\"><div class=\"nf-h-box nf-follow-comment-frame\">");
jade_mixins["add_comments"](val_sub, val_sub.comment_id, val[0].comment_id);
buf.push("</div></div>");
}
    }

  }
}).call(this);

    }

  }
}).call(this);

}
buf.push("</div>");}.call(this,"comments_array" in locals_for_with?locals_for_with.comments_array:typeof comments_array!=="undefined"?comments_array:undefined,"undefined" in locals_for_with?locals_for_with.undefined:typeof undefined!=="undefined"?undefined:undefined));;return buf.join("");
}