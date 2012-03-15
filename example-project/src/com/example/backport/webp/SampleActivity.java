package com.example.backport.webp;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.InputStream;

import android.app.Activity;
import android.content.Intent;
import android.database.Cursor;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.net.Uri;
import android.os.Bundle;
import android.os.Environment;
import android.provider.MediaStore;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;

import android.backport.webp.WebPFactory;

public class SampleActivity extends Activity {
	private final int REQUEST_CODE__IMAGE_SELECTED = 1;
	private final int REQUEST_CODE__IMAGE_CAPTURED = 2;

	private static byte[] streamToBytes(InputStream is) {
		ByteArrayOutputStream os = new ByteArrayOutputStream(1024);
		byte[] buffer = new byte[1024];
		int len;
		try {
			while ((len = is.read(buffer)) >= 0) {
				os.write(buffer, 0, len);
			}
		} catch (java.io.IOException e) {
		}
		return os.toByteArray();
	}
	
	ImageView _imageView = null;
	
	File _captureDestination = null;

	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);

		_imageView = (ImageView) findViewById(R.id.imageView);

		final Button loadEmbeddedImageButton = (Button) findViewById(R.id.loadEmbeddedImage);
		loadEmbeddedImageButton.setOnClickListener(new View.OnClickListener() {
			public void onClick(View v) {
				InputStream rawImageStream = getResources().openRawResource(R.raw.image);
				byte[] data = streamToBytes(rawImageStream);
				final Bitmap webpBitmap = WebPFactory.nativeDecodeByteArray(
						data, null);
				_imageView.setImageBitmap(webpBitmap);
			}
		});

		final Button findAndLoadImageButton = (Button) findViewById(R.id.findAndLoadImage);
		findAndLoadImageButton.setOnClickListener(new View.OnClickListener() {
			public void onClick(View v) {
				Intent intent = new Intent();
				intent.setType("image/*");
				intent.setAction(Intent.ACTION_GET_CONTENT);
				startActivityForResult(Intent.createChooser(intent, "Select Picture"),
						REQUEST_CODE__IMAGE_SELECTED);
			}
		});

		final Button captureAndLoadImageButton = (Button) findViewById(R.id.captureImage);
		captureAndLoadImageButton
				.setOnClickListener(new View.OnClickListener() {
					public void onClick(View v) {
						Intent intent = new Intent(android.provider.MediaStore.ACTION_IMAGE_CAPTURE);
						_captureDestination = new File(Environment.getExternalStorageDirectory(), "camera.jpg");
						intent.putExtra(MediaStore.EXTRA_OUTPUT, Uri.fromFile(_captureDestination));

						startActivityForResult(intent, REQUEST_CODE__IMAGE_CAPTURED);
					}
				});
	}

	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent intent) {
		super.onActivityResult(requestCode, resultCode, intent);

		switch (requestCode) {
		case REQUEST_CODE__IMAGE_SELECTED:
			if (resultCode == RESULT_OK) {
				Uri selectedImage = intent.getData();
				String[] filePathColumn = { MediaStore.Images.Media.DATA };

				Cursor cursor = getContentResolver().query(selectedImage,
						filePathColumn, null, null, null);
				cursor.moveToFirst();

				int columnIndex = cursor.getColumnIndex(filePathColumn[0]);
				String filePath = cursor.getString(columnIndex);
				cursor.close();

				Bitmap selectedBitmap = BitmapFactory.decodeFile(filePath);
				byte[] webpImageData = WebPFactory.nativeEncodeBitmap(selectedBitmap, 100);
				Bitmap webpBitmap = WebPFactory.nativeDecodeByteArray(webpImageData, null);
				_imageView.setImageBitmap(webpBitmap);
			}
			break;
		case REQUEST_CODE__IMAGE_CAPTURED:
			if (resultCode == RESULT_OK) {
				Bitmap selectedBitmap = BitmapFactory.decodeFile(_captureDestination.getAbsolutePath());
				byte[] webpImageData = WebPFactory.nativeEncodeBitmap(selectedBitmap, 100);
				Bitmap webpBitmap = WebPFactory.nativeDecodeByteArray(webpImageData, null);
				_imageView.setImageBitmap(webpBitmap);
				
				_captureDestination = null;
			}
			break;
		}
	}
}