int gnTotalItems = 0;
TDateTime gStart = Now; 

THmsScriptMediaItem CreateFolder(THmsScriptMediaItem Folder, string sName, string sLink, string sImg='', string sDesc='') 
{
  THmsScriptMediaItem Item = Folder.AddFolder(sLink); // Создаём папку с указанной ссылкой
  Item[mpiTitle     ] = sName; // Присваиваем наименование
  Item[mpiThumbnail ] = sImg;  // Картинка
  Item[mpiComment ] = sDesc;  // Описание
  Item[mpiCreateDate] = DateTimeToStr(IncTime(gStart, 0, 0, -gnTotalItems, 0));
  gnTotalItems++;             // Увеличиваем счетчик созданных элементов
  return Item;                // Возвращаем созданный объект
}

void CreateLinksToVideo(THmsScriptMediaItem item)
{
  string sUrl = item[mpiFilePath];
  string sData = HmsDownloadURL(sUrl);
  int num=1;
  
  TRegExpr REVid = TRegExpr.Create('\"(.*numb=[0-9]+)', PCRE_UNGREEDY);
  if (REVid.Search(sData)) 
  {
    item.DeleteChildItems;
    
    do
    {
      string fUrl = REVid.Match(1);
      string fName   = Str(num) +" - "+ item[mpiTitle];
      num=num+1;
      
      AddMediaItem(item, fName, fUrl);
    }
    while (REVid.SearchAgain());
  }
  REVid.Free;
}


THmsScriptMediaItem AddMediaItem(THmsScriptMediaItem Folder, string sTitle, string sLink, string sGrp='') {
  THmsScriptMediaItem Item = HmsCreateMediaItem(sLink, Folder.ItemID, sGrp); // Создаём ссылку
  Item[mpiTitle     ] = sTitle;       // Наименование
  Item[mpiCreateDate] = DateTimeToStr(IncTime(gStart, 0, 0, -gnTotalItems, 0));
  Item[mpiThumbnail] = Folder[mpiThumbnail];
  Item[mpiComment] = Folder[mpiComment];
  Item[mpiDirectLink] = True;
  return Item;                       // Возвращаем созданный объект
}


{
  FolderItem.DeleteChildItems(); // Удаляем созданное ранее содержимое
  
  string path = CurrentMediaItem.Properties[mpiFilePath] ; // получаем из настроек
  string sData = HmsDownloadURL(path);
  string fUrl, fName, fImg, fDescr;
  
  TRegExpr RE = TRegExpr.Create('href=\"(.*)\"><font.*>(.*)<\/font>', 0);
  TRegExpr REImg = TRegExpr.Create('<img[^>]+src="(http://portal.idknet.com/modules/mydownloads.*?)"', 0);
  TRegExpr REDesc = TRegExpr.Create('Описание: </b></td><td>([^<]+)', 0);
  if (RE.Search(sData)) 
  {
    REImg.Search(sData);
    REDesc.Search(sData);
    do
      {
        fUrl = RE.Match(1);
        fName   = RE.Match(2);
        
        fImg = REImg.Match(1);
        fDescr = REDesc.Match(1);
        
        THmsScriptMediaItem item = CreateFolder(FolderItem, fName, fUrl, fImg, fDescr);
        CreateLinksToVideo(item);
        
        //break;
        
        REImg.SearchAgain();
        REDesc.SearchAgain();
      }
    while (RE.SearchAgain() );
  }
  RE.Free;
  REImg.Free;
  REDesc.Free;
}